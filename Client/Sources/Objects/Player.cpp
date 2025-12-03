#include "pch.h"
#include "Player.h"
#include "CCore.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "MouseMgr.h"
#include "CameraMgr.h"
#include "SceneMgr.h"
#include "CCamera.h"
#include "CScene.h"
#include "Ascent.h"


Player::Player()
    : CObject() {

    // --- 1. Model / Animation 로드 (기존 Player 사용) ---
    string modelPath = "first2";
    m_pModel = new IModel<AnimModel>(modelPath);

    AnimModel* currModel = m_pModel->GetModel(); // 모델에서 본 정보를 가져옴

    m_pIdleAnim = new Animation("Models/first2/firstIdle.gltf", currModel);
    m_pRunAnim = new Animation("Models/first2/first.gltf", currModel);
    
    m_pAnimator = new Animator(m_pIdleAnim);

    // --- 2. Physics 변수 초기화 (신규 Player 적용) ---
    m_gravity = -9.81f;
    m_velocity = vec3(0.0f);
    m_isOnGround = true; // (바닥에 서있다고 가정)
    m_jumpVelocity = 3.0f;

    // --- 3. CObject 기본값 설정 (기본 Render 설정을 위함) ---
    // main.cpp의 하드코딩을 대신해 CObject의 기본값으로 설정
    setScale(vec3(0.3f, 0.3f, 0.3f));
    setRotation(vec3(0, 0.0f, 0.0f));; // X축 0도 (필요시 수정)
    // (translate(0, -1, 0)은 position.y로 중점 보정 처리)
}

Player::~Player() {
    if (m_pModel) delete m_pModel;
    if (m_pAnimator) delete m_pAnimator;
    if (m_pIdleAnim) delete m_pIdleAnim;
    if (m_pRunAnim) delete m_pRunAnim;
}

bool IntersectRayTriangle(const glm::vec3& orig, const glm::vec3& dir,
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
    float& t) 
{
    const float EPSILON = 0.0000001f;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = glm::cross(dir, edge2);
    a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) return false;
    f = 1.0f / a;
    s = orig - v0;
    u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f) return false;
    t = f * glm::dot(edge2, q);
    return t > EPSILON;
}

// 점과 삼각형 사이의 가장 가까운 점 구하기
glm::vec3 ClosestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ap = p - a;
    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    glm::vec3 bp = p - b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    glm::vec3 cp = p - c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return a + w * ac;
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b);
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

void Player::PreventTunneling(const std::vector<glm::vec3>& mapTriangles, glm::vec3& moveVec) {
    float moveDist = glm::length(moveVec);
    if (moveDist <= 0.0001f) return;

    glm::vec3 dir = glm::normalize(moveVec);
    // 캡슐의 허리(중심) 높이에서 검사
    glm::vec3 rayOrigin = this->position + glm::vec3(0.0f, m_height * 0.5f, 0.0f);
    
    // 검사 거리: 이동할 거리 + 반지름 (여유분)
    // 반지름만큼 더 검사해야 몸체가 닿기 전에 감지 가능
    float checkDist = moveDist + m_radius; 

    float closestT = 9999.0f;
    bool hit = false;
    glm::vec3 hitNormal(0.0f);

    for (size_t i = 0; i < mapTriangles.size(); i += 3) {
        const glm::vec3& v0 = mapTriangles[i];
        const glm::vec3& v1 = mapTriangles[i + 1];
        const glm::vec3& v2 = mapTriangles[i + 2];

        // 벽인지 확인 (바닥 제외)
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        if (normal.y >= 0.5f) continue; 

        // 이동 방향과 벽이 마주보고 있는지 확인 (뒷면 제외)
        if (glm::dot(dir, normal) >= 0.0f) continue;

        float t;
        if (IntersectRayTriangle(rayOrigin, dir, v0, v1, v2, t)) {
            if (t < closestT) {
                closestT = t;
                hit = true;
                hitNormal = normal;
            }
        }
    }

    // 벽 충돌이 예상되면
    if (hit && closestT < checkDist) {
        // 벽 표면에서 반지름만큼 뺀 거리까지만 이동 가능
        float safeDist = closestT - m_radius;
        if (safeDist < 0.0f) safeDist = 0.0f; // 이미 파묻혔다면 제자리

        // 이동 벡터를 안전 거리로 제한 (벽 바로 앞에서 멈춤)
        // 이후 ResolveWallCollision이 미끄러짐(Sliding)을 처리함
        moveVec = dir * safeDist;
    }
}

void Player::Update()
{
    double dt = DT;

    // ---------------------------------------------------------
    // 1. [디버그] V키 입력 처리 (히트박스 시각화 토글)
    // ---------------------------------------------------------
    
    if (KeyMgr::Instance()->getKeyState(KEY::V) == KEY_TYPE::TAP) {
        m_bThirdPerson = !m_bThirdPerson;
        std::cout << "[Camera] Mode switched to: " << (m_bThirdPerson ? "3rd Person" : "1st Person") << std::endl;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::B) == KEY_TYPE::TAP) {
        m_bShowHitbox = !m_bShowHitbox;
        // 씬에서 Ascent(맵) 객체를 찾아 히트박스 시각화 상태 동기화
        CScene* pScene = SceneMgr::Instance()->getScene();
        if (pScene) {
            const auto& objs = pScene->GetObjects(GROUP_TYPE::DEFAULT);
            for (auto obj : objs) {
                Ascent* ascent = dynamic_cast<Ascent*>(obj);
                if (ascent) {
                    ascent->SetShowHitbox(m_bShowHitbox);
                }
            }
        }
    }

    // ---------------------------------------------------------
    // 2. [최적화] 맵 데이터 참조 가져오기 (복사 방지)
    // ---------------------------------------------------------
    const std::vector<glm::vec3>* pMapTriangles = nullptr;
    
    CScene* pScene = SceneMgr::Instance()->getScene();
    if (pScene) {
        const auto& objs = pScene->GetObjects(GROUP_TYPE::DEFAULT);
        for (auto obj : objs) {
            Ascent* ascent = dynamic_cast<Ascent*>(obj);
            if (ascent) {
                // [중요] 벡터를 복사하지 않고 주소값만 가져옵니다.
                pMapTriangles = &ascent->GetAllTriangles();
                break;
            }
        }
    }

    // ---------------------------------------------------------
    // 3. 이동 입력 및 속도 계산
    // ---------------------------------------------------------
    CCamera* cam = CameraMgr::Instance()->getMainCamera();
    glm::vec3 tar = cam->target;
    glm::vec3 pos = cam->position;

    // 카메라 기준 이동 방향 계산
    glm::vec3 viewVec = tar - pos;
    viewVec.y = 0.0f;
    viewVec = glm::normalize(viewVec);
    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
    rightVec = glm::normalize(rightVec);

    bool isMoving = false;
    float moveSpeed = 3.0f;
    glm::vec3 moveDir = glm::vec3(0.0f);

    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) { moveDir += viewVec; isMoving = true; }
    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) { moveDir -= viewVec; isMoving = true; }
    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) { moveDir += rightVec; isMoving = true; }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) { moveDir -= rightVec; isMoving = true; }

    if (isMoving) {
        if (glm::length(moveDir) > 0.001f) moveDir = glm::normalize(moveDir);
        m_velocity.x = moveDir.x * moveSpeed;
        m_velocity.z = moveDir.z * moveSpeed;
    }
    else {
        m_velocity.x = 0.0f;
        m_velocity.z = 0.0f;
    }

    // 점프 처리
    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD /*&& m_isOnGround*/) {
        m_isOnGround = false;
        m_velocity.y = m_jumpVelocity;
    }

    // ---------------------------------------------------------
    // 4. 물리 시뮬레이션
    // ---------------------------------------------------------
    
    // (1) 중력 적용
    ApplyGravity();

    // (2) 이번 프레임의 이동 벡터 계산
    glm::vec3 moveVec = m_velocity * (float)dt;

    // (3) [중요] 터널링 방지 (Predictive Raycast)
    // 이동하려는 거리가 벽을 뚫을 정도로 길다면, 벽 앞까지만 이동하도록 벡터를 제한합니다.
    if (pMapTriangles && !pMapTriangles->empty()) {
        PreventTunneling(*pMapTriangles, moveVec);
    }

    // (4) 예상 위치 계산
    glm::vec3 nextPos = this->position + moveVec;

    // (5) [중요] 벽 충돌 해결 (Sliding & Iteration)
    // 벽에 닿았을 때 미끄러지도록 처리하며, 구석에 끼는 것을 방지하기 위해 반복 수행합니다.
    if (pMapTriangles && !pMapTriangles->empty()) {
        int iterations = 3; 
        for (int i = 0; i < iterations; ++i) {
            ResolveWallCollision(*pMapTriangles, nextPos);
        }
    }

    // (6) 최종 위치 반영
    this->position = nextPos;

    // (7) [중요] 바닥 충돌 처리 (Ground Snap)
    // 발 위치를 바닥 높이에 맞추고 착지 상태(m_isOnGround)를 갱신합니다.
    if (pMapTriangles && !pMapTriangles->empty()) {
        CheckFloorCollision(*pMapTriangles);
    }

    // ---------------------------------------------------------
    // 5. 기타 로직 (네트워크, 애니메이션, 카메라)
    // ---------------------------------------------------------
    
    // 발사 패킷 전송
    if (KeyMgr::Instance()->getKeyState(KEY::E) == KEY_TYPE::TAP) {
        CCamera* cam = CameraMgr::Instance()->getMainCamera();
        glm::vec3 fireDir = glm::normalize(cam->target - cam->position);
        C2S_FireAction firePkt = BuildFirePacket(cam->position, fireDir, -1);
        
        if (g_pNetwork) {
            g_pNetwork->SendFire(firePkt);
        }
    }

    // 이동 패킷 생성 (필요 시 전송 로직 추가)
    C2S_MovementUpdate movementPkt = BuildMovementPacket();

    // 애니메이션 상태 업데이트
    if (isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pRunAnim) 
            m_pAnimator->PlayAnimation(m_pRunAnim);
    }
    else if (!isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pIdleAnim) 
            m_pAnimator->PlayAnimation(m_pIdleAnim);
    }
    
    if (m_pAnimator) {
        m_pAnimator->UpdateAnimation(DT);
    }

    // 카메라 위치 동기화 (눈 높이 설정)
    if (CameraMgr::Instance()->getMainCamera()) {
        CCamera* pCam = CameraMgr::Instance()->getMainCamera();
        
        // 현재 카메라가 바라보는 방향 벡터 (회전값 유지)
        glm::vec3 camDir = glm::normalize(pCam->target - pCam->position);
        
        // 플레이어의 눈 위치 (캡슐 상단)
        glm::vec3 eyePos = this->position + glm::vec3(0.0f, m_height * 10.0f, 0.0f);
        
        // 최종 카메라 위치 계산
        glm::vec3 finalCamPos;

        if (m_bThirdPerson) {
            // [3인칭] 눈 위치에서 시선 반대 방향으로 2.5m 뒤로 이동
            float distance = 2.5f; 
            // 약간 위로 올려서(Up Vector) 숄더뷰 느낌을 낼 수도 있음
            // finalCamPos = eyePos - (camDir * distance) + glm::vec3(0, 0.5f, 0); 
            finalCamPos = eyePos - (camDir * distance);
        }
        else {
            // [1인칭] 눈 위치와 동일
            finalCamPos = eyePos;
        }
        
        pCam->position = finalCamPos;
        // 타겟은 "카메라 위치 + 바라보던 방향"으로 설정하여 회전각 유지
        pCam->target = finalCamPos + camDir;
    }
}


void Player::Render() {
    if (m_pModel) { // [Refactored]
        glUseProgram(CCore::Instance()->shaderProgramID2);
        {
            AnimModel* currModel = m_pModel->GetModel(); // [Refactored]

            // CObject의 P, R, S로 Model Matrix 계산
            glm::mat4 modelMat = glm::mat4(1.0f);
            glm::mat4 T = glm::translate(glm::mat4(1.f), position);
            glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z))));
            glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
            modelMat = T * R * S;

            // (main.cpp에서 수행하던 발 위치 보정을 위한 translate)
            modelMat = glm::translate(modelMat, vec3(0.0f, 0.1f, 0.0f));

            // PVM 등 셰이더 Uniform 전송
            mat4 projMat = CameraMgr::Instance()->getMainCamera()->getProjectionMatrix();
            mat4 view = CameraMgr::Instance()->getMainCamera()->getViewMatrix();
            glm::mat4 PVM = projMat * view * modelMat;
            glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));

            GLuint loc_modelMat = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "modelMat");
            GLuint loc_PVM = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "PVM");
            GLuint loc_normalMat = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMat");
            glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
            glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
            glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

            // Bone(뼈) Uniform 전송
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices();
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++)
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

            // 텍스처 Sampler 설정
            GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
            GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
            glUniform1i(loc_diffuseSampler, 0);
            glUniform1i(loc_normalSampler, 1);

            // 렌더링 수행
            currModel->Render();

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
                std::cout << "error : " << error << std::endl;
        }
        if (m_bShowHitbox) {
            RenderHitbox();
        }
    }
}

void Player::RenderHitbox() {
    glUseProgram(0); 
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // *** 중요: 카메라가 내부에 있어도 보이도록 CULL_FACE 끄기 ***
    glDisable(GL_CULL_FACE); 

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getProjectionMatrix()));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(CameraMgr::Instance()->getMainCamera()->getViewMatrix()));

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    // 캡슐 색상 (초록색)
    glColor3f(0.0f, 1.0f, 0.0f);

    // 하단 구
    glPushMatrix();
    glTranslatef(0.0f, m_radius, 0.0f);
    glutWireSphere(m_radius, 16, 16);
    glPopMatrix();

    // 상단 구
    glPushMatrix();
    glTranslatef(0.0f, m_height - m_radius, 0.0f);
    glutWireSphere(m_radius, 16, 16);
    glPopMatrix();

    // 몸통 (원기둥 대신 선으로 연결 - WireCylinder는 축 맞추기 번거로움)
    glBegin(GL_LINES);
    // 4방향 기둥 선
    glVertex3f(m_radius, m_radius, 0);   glVertex3f(m_radius, m_height - m_radius, 0);
    glVertex3f(-m_radius, m_radius, 0);  glVertex3f(-m_radius, m_height - m_radius, 0);
    glVertex3f(0, m_radius, m_radius);   glVertex3f(0, m_height - m_radius, m_radius);
    glVertex3f(0, m_radius, -m_radius);  glVertex3f(0, m_height - m_radius, -m_radius);
    glEnd();

    glPopMatrix();

    // 상태 복구
    glEnable(GL_CULL_FACE); // 다시 켜기
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
}

C2S_MovementUpdate Player::BuildMovementPacket()
{
    C2S_MovementUpdate pkt;

    // 메시지 시퀀스 번호 증가 및 할당
    m_movementSeq++;
    pkt.msgSeq = m_movementSeq;

    // PlayerID 임시로 0 처리
    pkt.playerId = 0;

    // 현재 위치, 회전, 속도 정보 패킷에 담기
    pkt.position.x = position.x;
    pkt.position.y = position.y;
    pkt.position.z = position.z;

    pkt.rotation.x = rotation.x;
    pkt.rotation.y = rotation.y;
    pkt.rotation.z = rotation.z;

    pkt.velocity.x = m_velocity.x;
    pkt.velocity.y = m_velocity.y;
    pkt.velocity.z = m_velocity.z;

    // 클라이언트 시간 타임스탬프 설정
    pkt.clientTime = (float)TimeMgr::Instance()->getCurrTime();


    return pkt;
}

C2S_FireAction Player::BuildFirePacket(const vec3& fireOrigin, const vec3& fireDirection, PlayerID targetID)
{
    C2S_FireAction pkt;

    // 메시지 시퀀스 번호 증가 및 할당
    m_fireSeq++;
    pkt.msgSeq = m_fireSeq;

    // PlayerID 임시로 0 처리
    pkt.playerId = 0;

    // 발사 원점 및 방향 설정
    pkt.fireOrigin.x = fireOrigin.x;
    pkt.fireOrigin.y = fireOrigin.y;
    pkt.fireOrigin.z = fireOrigin.z;

    pkt.fireDirection.x = fireDirection.x;
    pkt.fireDirection.y = fireDirection.y;
    pkt.fireDirection.z = fireDirection.z;

    // 클라이언트 시간 타임스탬프 설정
    pkt.clientTime = (float)TimeMgr::Instance()->getCurrTime();

    pkt.hitPlayerID = targetID;

    return pkt;
}

void Player::ApplyGravity() {
    double dt = DT;
    if (!m_isOnGround) {
        m_velocity.y += m_gravity * dt;
    }
}

void Player::CheckFloorCollision(const std::vector<glm::vec3>& mapTriangles) {
    // 캡슐의 바닥 중심에서 시작 (position)
    // 약간 위에서 아래로 쏴서 묻혀있는 경우도 처리
    glm::vec3 rayOrigin = this->position;
    rayOrigin.y += m_radius; // 반지름만큼 위에서
    glm::vec3 rayDir = glm::vec3(0, -1, 0);

    float closestT = 9999.0f;
    bool hit = false;

    for (size_t i = 0; i < mapTriangles.size(); i += 3) {
        glm::vec3 v0 = mapTriangles[i];
        glm::vec3 v1 = mapTriangles[i + 1];
        glm::vec3 v2 = mapTriangles[i + 2];
        
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        if (normal.y < 0.5f) continue; // 경사가 급하면 바닥 아님

        float t;
        if (IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, t)) {
            if (t < closestT) {
                closestT = t;
                hit = true;
            }
        }
    }

    if (hit) {
        float floorY = rayOrigin.y - closestT;
        float distToFloor = this->position.y - floorY;

        // 바닥에 매우 근접하거나 살짝 뚫었을 때 착지 처리
        if (distToFloor <= 0.1f && distToFloor >= -0.5f && m_velocity.y <= 0.0f) {
            this->position.y = floorY;
            m_velocity.y = 0.0f;
            m_isOnGround = true;
        } else {
             m_isOnGround = false;
        }
    } else {
        m_isOnGround = false;
    }
}

void Player::ResolveWallCollision(const std::vector<glm::vec3>& mapTriangles, glm::vec3& nextPos) {
    // 캡슐 정의: 
    // Segment A (바닥 구 중심): nextPos + (0, radius, 0)
    // Segment B (천장 구 중심): nextPos + (0, height - radius, 0)
    
    glm::vec3 segA = nextPos + glm::vec3(0.0f, m_radius, 0.0f);
    glm::vec3 segB = nextPos + glm::vec3(0.0f, m_height - m_radius, 0.0f);

    for (size_t i = 0; i < mapTriangles.size(); i += 3) {
        glm::vec3 v0 = mapTriangles[i];
        glm::vec3 v1 = mapTriangles[i + 1];
        glm::vec3 v2 = mapTriangles[i + 2];

        // 바닥 평면은 제외 (CheckFloorCollision이 처리)
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        if (normal.y > 0.7f) continue; 

        // [간이 캡슐 충돌]
        // 1. 삼각형의 높이(Y)를 캡슐 세그먼트 범위 내로 클램핑하여 "가장 가까울 법한 높이"를 찾음
        glm::vec3 triCenter = (v0 + v1 + v2) / 3.0f;
        float t = glm::clamp((triCenter.y - segA.y) / (segB.y - segA.y), 0.0f, 1.0f);
        
        // 2. 그 높이에서의 캡슐 축 상의 점 (Sphere Center 역할)
        glm::vec3 sphereCenter = segA + t * (segB - segA);

        // 3. 해당 구와 삼각형 간의 최단 거리 검사
        glm::vec3 closest = ClosestPointOnTriangle(sphereCenter, v0, v1, v2);
        glm::vec3 diff = sphereCenter - closest;
        
        // 벽 밀어내기는 수평(XZ)으로만 제한 (계단 현상 방지)
        diff.y = 0.0f; 
        
        float distSq = glm::dot(diff, diff);

        if (distSq > 0.0001f && distSq < m_radius * m_radius) {
            float dist = sqrt(distSq);
            glm::vec3 pushDir = diff / dist;
            float pushDist = m_radius - dist;
            
            // 위치 보정
            nextPos += pushDir * pushDist;
            
            // 캡슐 축도 같이 이동시켜서 다음 삼각형 검사에 반영
            segA += pushDir * pushDist;
            segB += pushDir * pushDist;
        }
    }
}