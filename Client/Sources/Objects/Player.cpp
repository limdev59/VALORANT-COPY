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



void Player::Update()
{
    double dt = DT;
    CCamera* cam = CameraMgr::Instance()->getMainCamera();
    glm::vec3 tar = cam->target;
    glm::vec3 pos = cam->position;

    // --- 1. 이동 입력 처리 ---
    glm::vec3 viewVec = tar - pos;
    viewVec.y = 0.0f;
    viewVec = glm::normalize(viewVec);
    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
    rightVec = glm::normalize(rightVec);

    bool isMoving = false;
    float moveSpeed = 3.0f;
    glm::vec3 moveDir = glm::vec3(0.0f);

    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) {
        moveDir += viewVec;
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) {
        moveDir -= viewVec;
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) {
        moveDir += rightVec;
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
        moveDir -= rightVec;
        isMoving = true;
    }

    // --- 2. 속도 및 점프 설정 ---
    if (isMoving) {
        moveDir = glm::normalize(moveDir);
        m_velocity.x = moveDir.x * moveSpeed;
        m_velocity.z = moveDir.z * moveSpeed;
    }
    else {
        m_velocity.x = 0.0f;
        m_velocity.z = 0.0f;
    }

    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD && m_isOnGround) {
        m_isOnGround = false;
        m_velocity.y = m_jumpVelocity;
    }

    m_isOnGround = false;
    // 중력 적용
    ApplyGravity();

    // --- 3. 히트박스 기본 설정 ---
    hitboxCenter = glm::vec3(0.0f);
    hitboxSize = scale;

    // --- 3. 예상 위치 계산 ---
    glm::vec3 nextPosition = this->position + (m_velocity * (float)dt);

    // --- 4. 충돌 처리 (CScene 의존) ---
    CScene* pCurScene = SceneMgr::Instance()->getScene();
    if (!pCurScene) return;

    const vector<CObject*>& mapObjects = pCurScene->GetObjects(GROUP_TYPE::DEFAULT);
    const vector<CObject*>& enemyObjects = pCurScene->GetObjects(GROUP_TYPE::ENEMY);

    glm::vec3 originalPos = this->position;
    bool isGroundedThisFrame = false;

    auto buildPlayerAABB = [&](const glm::vec3& pos) {
        glm::vec3 center = pos + hitboxCenter;
        glm::vec3 half = hitboxSize * 0.5f;
        return std::make_pair(center - half, center + half);
    };

    auto buildObjectAABB = [&](const CObject& obj) {
        glm::vec3 center = obj.getPosition() + obj.getHitboxCenter();
        glm::vec3 half = obj.getHitboxSize() * 0.5f;
        return std::make_pair(center - half, center + half);
    };

    auto intersects = [&](const std::pair<glm::vec3, glm::vec3>& a, const std::pair<glm::vec3, glm::vec3>& b) {
        return !(a.second.x < b.first.x || a.first.x > b.second.x ||
                 a.second.y < b.first.y || a.first.y > b.second.y ||
                 a.second.z < b.first.z || a.first.z > b.second.z);
    };

    auto collidesWith = [&](const glm::vec3& candidatePos, const CObject* other) {
        const auto playerBox = buildPlayerAABB(candidatePos);
        if (const Ascent* ascent = dynamic_cast<const Ascent*>(other)) {
            for (const auto& box : ascent->GetWorldColliders()) {
                if (intersects(playerBox, box)) {
                    return true;
                }
            }
            return false;
        }

        return intersects(playerBox, buildObjectAABB(*other));
    };

    // Y축(중력/바닥) 충돌 검사
    glm::vec3 candidatePos = glm::vec3(originalPos.x, nextPosition.y, originalPos.z);

    for (CObject* other : mapObjects) {
        if (collidesWith(candidatePos, other)) {
            if (m_velocity.y < 0) {
                isGroundedThisFrame = true;
            }
            m_velocity.y = 0;
            candidatePos.y = originalPos.y;
            break;
        }
    }
    for (CObject* other : enemyObjects) {
        if (collidesWith(candidatePos, other)) {
            m_velocity.y = 0;
            candidatePos.y = originalPos.y;
            break;
        }
    }
    nextPosition.y = candidatePos.y;

    // X축(좌우) 충돌 검사
    candidatePos = glm::vec3(nextPosition.x, nextPosition.y, originalPos.z);
    for (CObject* other : mapObjects) {
        if (collidesWith(candidatePos, other)) {
            m_velocity.x = 0;
            candidatePos.x = originalPos.x;
            break;
        }
    }
    for (CObject* other : enemyObjects) {
        if (collidesWith(candidatePos, other)) {
            m_velocity.x = 0;
            candidatePos.x = originalPos.x;
            break;
        }
    }
    nextPosition.x = candidatePos.x;

    // Z축(앞뒤) 충돌 검사
    candidatePos = glm::vec3(nextPosition.x, nextPosition.y, nextPosition.z);
    for (CObject* other : mapObjects) {
        if (collidesWith(candidatePos, other)) {
            m_velocity.z = 0;
            candidatePos.z = originalPos.z;
            break;
        }
    }
    for (CObject* other : enemyObjects) {
        if (collidesWith(candidatePos, other)) {
            m_velocity.z = 0;
            candidatePos.z = originalPos.z;
            break;
        }
    }
    nextPosition.z = candidatePos.z;

    m_isOnGround = isGroundedThisFrame;

    // 최종 위치 반영
    setPosition(nextPosition);

    // --- 5. 액션 패킷 처리 (Fire) ---
    if (KeyMgr::Instance()->getKeyState(KEY::E) == KEY_TYPE::TAP) {
        CCamera* cam = CameraMgr::Instance()->getMainCamera();
        glm::vec3 camPos = cam->position;
        glm::vec3 camTarget = cam->target;

        glm::vec3 fireOrigin = camPos;
        glm::vec3 fireDirection = glm::normalize(camTarget - camPos);

        PlayerID hitID = -1;

        CScene* pScene = SceneMgr::Instance()->getScene();
        if (pScene) {
            const vector<CObject*>& enemies = pScene->GetObjects(GROUP_TYPE::ENEMY);
            float minDist = 1000.0f; // 사거리 제한

            for (CObject* enemyObj : enemies) {
                glm::vec3 toEnemy = enemyObj->getPosition() - fireOrigin;
                float dist = glm::length(toEnemy);

                // 방향이 거의 일치하고(내적 > 0.95), 사거리 내에 있다면
                if (dist < minDist && glm::dot(glm::normalize(toEnemy), fireDirection) > 0.99f) {
                    minDist = dist;
                    hitID = 999;
                }
            }
        }

        C2S_FireAction firePkt = BuildFirePacket(fireOrigin, fireDirection, hitID);
        std::cout << "[Fire] FireAction Packet Built! Seq: " << firePkt.msgSeq << std::endl;
        if (g_pNetwork) {
            g_pNetwork->SendFire(firePkt);
            std::cout << ">>> [Client] SendFire() Called! Packet sent to server. <<<" << std::endl;
        }
        else {
            std::cout << "!!! [Client] Error: g_pNetwork is NULL. Cannot send packet. !!!" << std::endl;
        }
    }

    // 이동 패킷 생성
    C2S_MovementUpdate movementPkt = BuildMovementPacket();

    // --- 6. 애니메이션 업데이트 ---
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

    // --- 7. 히트박스 업데이트 ---
    hitboxCenter = glm::vec3(0.0f);
    hitboxSize = scale;

    if (CameraMgr::Instance()->getMainCamera()) {
        CCamera* pCam = CameraMgr::Instance()->getMainCamera();
        glm::vec3 camDir = pCam->target - pCam->position;
        glm::vec3 eyePos = this->position + glm::vec3(0.0f, 0.5f, 0.0f);
        pCam->position = eyePos;
        pCam->target = eyePos + camDir;
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
    }
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
        m_velocity.y += m_gravity * dt; // 중력 가속도 적용
    }
    else {
        m_velocity.y = 0.0f;
    }
}