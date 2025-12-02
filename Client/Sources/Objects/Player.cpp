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

extern ClientNetwork* g_pNetwork;

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
    setHitbox(glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(0.6f, 1.8f, 0.6f), COLLIDER_TYPE::AABB);
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

    bool wasGrounded = m_isOnGround;
    m_isOnGround = false;

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

    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD && wasGrounded) {
        m_velocity.y = m_jumpVelocity;
    }

    // 중력 적용
    ApplyGravity(dt);

    // --- 3. 예상 위치 계산 ---
    glm::vec3 nextPosition = this->position + (m_velocity * (float)dt);

    // --- 4. 충돌 처리 (CScene 의존) ---
    CScene* pCurScene = SceneMgr::Instance()->getScene();
    if (!pCurScene) return;

    const vector<CObject*>& mapObjects = pCurScene->GetObjects(GROUP_TYPE::DEFAULT);

    Ascent* ascentMap = nullptr;
    for (CObject* mapObj : mapObjects) {
        ascentMap = dynamic_cast<Ascent*>(mapObj);
        if (ascentMap) break;
    }

    glm::vec3 resolvedPosition = nextPosition;
    if (ascentMap) {
        const auto walkables = ascentMap->GetWalkableColliders();
        const auto blockers = ascentMap->GetWallColliders();

        glm::vec3 verticalTarget(this->position.x, nextPosition.y, this->position.z);
        glm::vec3 verticalResolved = ResolveVerticalMovement(this->position, verticalTarget, walkables);

        glm::vec3 horizontalTarget(nextPosition.x, verticalResolved.y, nextPosition.z);
        resolvedPosition = ResolveHorizontalMovement(verticalResolved, horizontalTarget, blockers);
    }
    else {
        if (resolvedPosition.y <= -0.25f) {
            resolvedPosition.y = -0.25f;
            m_velocity.y = 0.0f;
            m_isOnGround = true;
        }
    }

    setPosition(resolvedPosition);

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

void Player::ApplyGravity(double dt) {
    m_velocity.y += m_gravity * static_cast<float>(dt);
}

std::pair<glm::vec3, glm::vec3> Player::ComputeAABBAtPosition(const glm::vec3& pos) const {
    glm::vec3 halfSize = hitboxSize * 0.5f;
    glm::vec3 center = pos + hitboxCenter;
    return { center - halfSize, center + halfSize };
}

std::pair<glm::vec3, glm::vec3> Player::GetWorldAABB() const {
    return ComputeAABBAtPosition(position);
}

bool Player::OverlapsXZ(const std::pair<glm::vec3, glm::vec3>& aabb, const Ascent::AABBCollider& collider) const {
    if (aabb.second.x < collider.min.x || aabb.first.x > collider.max.x) return false;
    if (aabb.second.z < collider.min.z || aabb.first.z > collider.max.z) return false;
    return true;
}

bool Player::CollidesWith(const glm::vec3& pos, const std::vector<Ascent::AABBCollider>& colliders) const {
    auto playerAABB = ComputeAABBAtPosition(pos);
    for (const auto& collider : colliders) {
        if (playerAABB.second.x < collider.min.x || playerAABB.first.x > collider.max.x) continue;
        if (playerAABB.second.y < collider.min.y || playerAABB.first.y > collider.max.y) continue;
        if (playerAABB.second.z < collider.min.z || playerAABB.first.z > collider.max.z) continue;
        return true;
    }
    return false;
}

glm::vec3 Player::ResolveVerticalMovement(const glm::vec3& currentPos, const glm::vec3& targetPos, const std::vector<Ascent::AABBCollider>& walkables) {
    glm::vec3 result = targetPos;
    auto currentAABB = ComputeAABBAtPosition(currentPos);
    auto nextAABB = ComputeAABBAtPosition(targetPos);
    glm::vec3 halfSize = hitboxSize * 0.5f;

    for (const auto& collider : walkables) {
        if (!OverlapsXZ(nextAABB, collider)) continue;

        if (m_velocity.y <= 0.0f && currentAABB.first.y >= collider.max.y && nextAABB.first.y <= collider.max.y) {
            float newY = collider.max.y - hitboxCenter.y + halfSize.y;
            result.y = newY;
            m_velocity.y = 0.0f;
            m_isOnGround = true;
            nextAABB = ComputeAABBAtPosition(result);
        }
        else if (m_velocity.y > 0.0f && currentAABB.second.y <= collider.min.y && nextAABB.second.y >= collider.min.y) {
            float newY = collider.min.y - hitboxCenter.y - halfSize.y;
            result.y = newY;
            m_velocity.y = 0.0f;
            nextAABB = ComputeAABBAtPosition(result);
        }
    }

    return result;
}

glm::vec3 Player::ResolveHorizontalMovement(const glm::vec3& currentPos, const glm::vec3& targetPos, const std::vector<Ascent::AABBCollider>& blockers) {
    glm::vec3 resolved = currentPos;

    glm::vec3 attempt = resolved;
    attempt.x = targetPos.x;
    if (!CollidesWith(attempt, blockers)) {
        resolved.x = attempt.x;
    }
    else {
        m_velocity.x = 0.0f;
    }

    attempt = resolved;
    attempt.z = targetPos.z;
    if (!CollidesWith(attempt, blockers)) {
        resolved.z = attempt.z;
    }
    else {
        m_velocity.z = 0.0f;
    }

    return resolved;
}
