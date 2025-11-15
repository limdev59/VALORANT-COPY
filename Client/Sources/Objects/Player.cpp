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


Player::Player()
    : CObject() {

    // --- 1. Model / Animation �ε� (���� Player ����) ---
    string modelPath = "first2";
    m_pModel = new IModel<AnimModel>(modelPath); // [Refactored]

    AnimModel* currModel = m_pModel->GetModel(); // ���ۿ��� ���� �� ������ ��������

    m_pIdleAnim = new Animation("Models/first2/firstIdle.gltf", currModel);
    m_pRunAnim = new Animation("Models/first2/first.gltf", currModel);
    
    m_pAnimator = new Animator(m_pIdleAnim);

    // --- 2. Physics ���� �ʱ�ȭ (�ű� Player ����) ---
    m_gravity = -9.81f;
    m_velocity = vec3(0.0f);
    m_isOnGround = true; // (�ٴڿ��� �����Ѵٰ� ����)
    m_jumpVelocity = 3.0f;

    // --- 3. CObject �⺻�� ���� (���� Render �������� ����) ---
    // main.cpp�� �ϵ��ڵ��� ���� CObject�� �⺻������ ����
    setScale(vec3(0.3f, 0.3f, 0.3f));
    setRotation(vec3(0, 0.0f, 0.0f)); // X�� 90�� ȸ��
    // (translate(0, -1, 0)�� position.y�� �߷����� ó��)
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

    // --- 1. �̵� ���� (�ű� Player.cpp) ---
    //ApplyGravity();

    glm::vec3 viewVec = tar - pos;
    viewVec.y = 0.0f;
    viewVec = glm::normalize(viewVec);
    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
    rightVec = glm::normalize(rightVec);

    bool isMoving = false;
    float moveSpeed = 10.0f;
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
        moveDir -= rightVec; // 'A' (����)
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
        moveDir += rightVec; // 'D' (������)
        isMoving = true;
    }


    // --- 2. ���� �� ���� ���� (�ű� Player.cpp) ---
    // (isJumping ��� m_isOnGround ��� ���� ���)
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

    // �� ������ �߷� ���� (Gravity() �Լ� ����)
    ApplyGravity();

    // --- 3. �ִϸ��̼� ���� ���� (���� Player ����) ---
    glm::vec3 nextPosition = this->position + (m_velocity * (float)dt);

    // --- 5. �浹 ó�� (CScene�� ���� �ʿ�) ---
    CScene* pCurScene = SceneMgr::Instance()->getScene();
    if (!pCurScene) return;
    // (����) std::vector<CObject*> collidables = m_pCurrentScene->GetCollidables();

    const vector<CObject*>& mapObjects = pCurScene->GetObjects(GROUP_TYPE::DEFAULT);
    const vector<CObject*>& enemyObjects = pCurScene->GetObjects(GROUP_TYPE::ENEMY);

    glm::vec3 originalPos = this->position;
    bool isGroundedThisFrame = false;

    // Y��(�߷�/����) �浹 ���� �˻�
    this->setPosition(glm::vec3(originalPos.x, nextPosition.y, originalPos.z));

    for (CObject* other : mapObjects) {
        if (this->CheckCollision(*other)) {
            if (m_velocity.y < 0) {
                isGroundedThisFrame = true;
            }
            m_velocity.y = 0;
            nextPosition.y = originalPos.y;
            break;
        }
    }

    // m_isOnGround ���� Ȯ��
    m_isOnGround = isGroundedThisFrame;

    // X��(�¿�) �浹 �˻�
    this->setPosition(glm::vec3(nextPosition.x, originalPos.y, originalPos.z));
    for (CObject* other : mapObjects) { // �ʰ� X�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.x = 0;
            nextPosition.x = originalPos.x;
            break;
        }
    }
    for (CObject* other : enemyObjects) { // ���� X�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.x = 0;
            nextPosition.x = originalPos.x;
            break;
        }
    }

    // Z��(�յ�) �浹 �˻�
    this->setPosition(glm::vec3(originalPos.x, originalPos.y, nextPosition.z));
    for (CObject* other : mapObjects) { // �ʰ� Z�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.z = 0;
            nextPosition.z = originalPos.z;
            break;
        }
    }
    for (CObject* other : enemyObjects) { // ���� Z�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.z = 0;
            nextPosition.z = originalPos.z;
            break;
        }
    }

    // ��ġ ���� ����
    this->setPosition(originalPos);

    setPosition(nextPosition); // 11/7 ������ġ - ����

    if (KeyMgr::Instance()->getKeyState(KEY::E) == KEY_TYPE::TAP) { // 11/11 ������ġ - ����
        // ī�޶��� �þ� ��������
        CCamera* cam = CameraMgr::Instance()->getMainCamera();
        glm::vec3 camPos = cam->position;
        glm::vec3 camTarget = cam->target;

        glm::vec3 fireOrigin = camPos;
        glm::vec3 fireDirection = glm::normalize(camTarget - camPos);

        C2S_FireAction firePkt = BuildFirePacket(fireOrigin, fireDirection);
        std::cout << "[Fire] FireAction Packet Built! Seq: " << firePkt.msgSeq << std::endl;
    }

    C2S_MovementUpdate movementPkt = BuildMovementPacket();

    glm::vec3 nextPosition = this->position + (m_velocity * (float)dt);

    // --- 5. �浹 ó�� (CScene�� ���� �ʿ�) ---
    CScene* pCurScene = SceneMgr::Instance()->getScene();
    if (!pCurScene) return;
    // (����) std::vector<CObject*> collidables = m_pCurrentScene->GetCollidables();

    const vector<CObject*>& mapObjects = pCurScene->GetObjects(GROUP_TYPE::DEFAULT);
    const vector<CObject*>& enemyObjects = pCurScene->GetObjects(GROUP_TYPE::ENEMY);

    glm::vec3 originalPos = this->position;
    bool isGroundedThisFrame = false;

    // Y��(�߷�/����) �浹 ���� �˻�
    this->setPosition(glm::vec3(originalPos.x, nextPosition.y, originalPos.z));

    for (CObject* other : mapObjects) {
        if (this->CheckCollision(*other)) {
            if (m_velocity.y < 0) {
                isGroundedThisFrame = true;
            }
            m_velocity.y = 0;
            nextPosition.y = originalPos.y;
            break;
        }
    }

    // m_isOnGround ���� Ȯ��
    m_isOnGround = isGroundedThisFrame;

    // X��(�¿�) �浹 �˻�
    this->setPosition(glm::vec3(nextPosition.x, originalPos.y, originalPos.z));
    for (CObject* other : mapObjects) { // �ʰ� X�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.x = 0;
            nextPosition.x = originalPos.x;
            break;
        }
    }
    for (CObject* other : enemyObjects) { // ���� X�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.x = 0;
            nextPosition.x = originalPos.x;
            break;
        }
    }

    // Z��(�յ�) �浹 �˻�
    this->setPosition(glm::vec3(originalPos.x, originalPos.y, nextPosition.z));
    for (CObject* other : mapObjects) { // �ʰ� Z�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.z = 0;
            nextPosition.z = originalPos.z;
            break;
        }
    }
    for (CObject* other : enemyObjects) { // ���� Z�� �˻�
        if (this->CheckCollision(*other)) {
            m_velocity.z = 0;
            nextPosition.z = originalPos.z;
            break;
        }
    }

    // ��ġ ���� ����
    this->setPosition(originalPos);

    setPosition(nextPosition);


    if (isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pRunAnim)
            m_pAnimator->PlayAnimation(m_pRunAnim);
    }
    else if (!isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pIdleAnim)
            m_pAnimator->PlayAnimation(m_pIdleAnim);
    }

    // (����: ���� �ִϸ��̼� ���µ� �߰� �ʿ�)
    if (m_pAnimator) {
        m_pAnimator->UpdateAnimation(DT);
    }


    // --- 4. ��Ʈ�ڽ� ������Ʈ (�ű� Player.cpp) ---
    hitboxCenter = position;
    hitboxSize = scale;

}


//void Player::Update()
//{
//    double dt = DT;
//    CCamera* cam = CameraMgr::Instance()->getMainCamera();
//    glm::vec3 tar = cam->target;
//    glm::vec3 pos = cam->position;
//
//    // --- 1. �̵� ���� (�ű� Player.cpp) ---
//    glm::vec3 viewVec = tar - pos;
//    viewVec.y = 0.0f;
//    viewVec = glm::normalize(viewVec);
//    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
//    rightVec = glm::normalize(rightVec);
//
//    bool isMoving = false;
//    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) {
//        position += viewVec * static_cast<float>(dt * 1.0);
//        isMoving = true;
//    }
//    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) {
//        position += rightVec * static_cast<float>(dt * 1.0);
//        isMoving = true;
//    }
//    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) {
//        position -= viewVec * static_cast<float>(dt * 1.0);
//        isMoving = true;
//    }
//    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
//        position -= rightVec * static_cast<float>(dt * 1.0);
//        isMoving = true;
//    }
//
//
//    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD && m_isOnGround) {
//        m_isOnGround = false;
//        m_velocity.y = m_jumpVelocity;
//    }
//
//    // �� ������ �߷� ���� (Gravity() �Լ� ����)
//    ApplyGravity();
//
//    // --- 3. �ִϸ��̼� ���� ���� (���� Player ����) ---
//    if (isMoving && m_isOnGround) {
//        if (m_pAnimator->GetCurrAnimation() != m_pRunAnim)
//            m_pAnimator->PlayAnimation(m_pRunAnim);
//    }
//    else if (!isMoving && m_isOnGround) {
//        if (m_pAnimator->GetCurrAnimation() != m_pIdleAnim)
//            m_pAnimator->PlayAnimation(m_pIdleAnim);
//    }
//    // (����: ���� �ִϸ��̼� ���µ� �߰� �ʿ�)
//    if (m_pAnimator) {
//        m_pAnimator->UpdateAnimation(DT);
//    }
//
//    // --- 4. ��Ʈ�ڽ� ������Ʈ (�ű� Player.cpp) ---
//    hitboxCenter = position;
//    hitboxSize = scale;
//
//}

void Player::Render() {
    if (m_pModel) { // [Refactored]
        glUseProgram(CCore::Instance()->shaderProgramID2);
        {
            AnimModel* currModel = m_pModel->GetModel(); // [Refactored]

            // CObject�� P, R, S�� Model Matrix ���
            glm::mat4 modelMat = glm::mat4(1.0f);
            glm::mat4 T = glm::translate(glm::mat4(1.f), position);
            glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z))));
            glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
            modelMat = T * R * S;

            // (main.cpp���� ������ �� ���� ������ translate)
            modelMat = glm::translate(modelMat, vec3(0.0f, 0.1f, 0.0f));

            // PVM �� ���̴� Uniform ����
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

            // Bone(��) Uniform ����
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices();
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++)
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

            // �ؽ�ó Sampler ����
            GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
            GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
            glUniform1i(loc_diffuseSampler, 0);
            glUniform1i(loc_normalSampler, 1);

            // �� ������
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

    // �޽��� ������ ���� �� ����
    m_movementSeq++;
    pkt.msgSeq = m_movementSeq;

    // PlayerID �ӽ÷� 0 ó��
    pkt.playerId = 0;

    // ���� ��ġ, ȸ��, �ӵ� ����.
    pkt.position.x = position.x;
    pkt.position.y = position.y;
    pkt.position.z = position.z;

    pkt.rotation.x = rotation.x;
    pkt.rotation.y = rotation.y;
    pkt.rotation.z = rotation.z;

    pkt.velocity.x = m_velocity.x;
    pkt.velocity.y = m_velocity.y;
    pkt.velocity.z = m_velocity.z;

    // Ŭ���̾�Ʈ �ð� Ÿ�ӽ����� ����
    pkt.clientTime = (float)TimeMgr::Instance()->getCurrTime();

    return pkt;
}

C2S_FireAction Player::BuildFirePacket(const vec3& fireOrigin, const vec3& fireDirection)
{
    C2S_FireAction pkt;

    // �޽��� ������ ���� �� ����
    m_fireSeq++;
    pkt.msgSeq = m_fireSeq;

    //  PlayerID �ӽ÷� 0 ó��
    pkt.playerId = 0;

    // ���� ��ġ, ȸ��, �ӵ� ����
    pkt.fireOrigin.x = fireOrigin.x;
    pkt.fireOrigin.y = fireOrigin.y;
    pkt.fireOrigin.z = fireOrigin.z;

    pkt.fireDirection.x = fireDirection.x;
    pkt.fireDirection.y = fireDirection.y;
    pkt.fireDirection.z = fireDirection.z;

    // Ŭ���̾�Ʈ �ð� Ÿ�ӽ����� ����
    pkt.clientTime = (float)TimeMgr::Instance()->getCurrTime();

    return pkt;
}

void Player::ApplyGravity() {
    double dt = DT;

    m_velocity.y += m_gravity * dt; // �߷� ���ӵ� ����
    position.y += m_velocity.y * dt; // y�� �ӵ��� ��ġ�� �ݿ�

    m_isOnGround = false; // �⺻���� ����

    // �ٴڰ� �浹 ó�� (���� �ϵ��ڵ��� ���� ����)
    if (position.x < -2.31563 && position.z < -6.84576) {
        if (position.y <= -0.5f) {
            position.y = -0.5f;
            m_velocity.y = 0.0f;
            m_isOnGround = true; // ���� ����
        }
    }
    else if (position.y <= -0.25f) {
        position.y = -0.25f;
        m_velocity.y = 0.0f;
        m_isOnGround = true; // ���� ����
    if (!m_isOnGround) {
        m_velocity.y += m_gravity * dt;
    }
}

//void Player::ApplyGravity() {
//    double dt = DT;
//
//    m_velocity.y += m_gravity * dt; // �߷� ���ӵ� ����
//    position.y += m_velocity.y * dt; // y�� �ӵ��� ��ġ�� �ݿ�
//
//    m_isOnGround = false; // �⺻���� ����
//
//    // �ٴڰ� �浹 ó�� (���� �ϵ��ڵ��� ���� ����)
//    if (position.x < -2.31563 && position.z < -6.84576) {
//        if (position.y <= -0.5f) {
//            position.y = -0.5f;
//            m_velocity.y = 0.0f;
//            m_isOnGround = true; // ���� ����
//        }
//    }
//    else if (position.y <= -0.25f) {
//        position.y = -0.25f;
//        m_velocity.y = 0.0f;
//        m_isOnGround = true; // ���� ����
//    }
//}
