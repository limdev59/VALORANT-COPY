#include "pch.h"
#include "Player.h"
#include "CCore.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "MouseMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"


Player::Player()
    : CObject() {

    // --- 1. Model / Animation ï¿½Îµï¿½ (ï¿½ï¿½ï¿½ï¿½ Player ï¿½ï¿½ï¿½ï¿½) ---
    string modelPath = "first2";
    m_pModel = new IModel<AnimModel>(modelPath); // [Refactored]

    AnimModel* currModel = m_pModel->GetModel(); // ï¿½ï¿½ï¿½Û¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    m_pIdleAnim = new Animation("Models/first2/firstIdle.gltf", currModel);
    m_pRunAnim = new Animation("Models/first2/first.gltf", currModel);
    
    m_pAnimator = new Animator(m_pIdleAnim);

    // --- 2. Physics ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­ (ï¿½Å±ï¿½ Player ï¿½ï¿½ï¿½ï¿½) ---
    m_gravity = -9.81f;
    m_velocity = vec3(0.0f);
    m_isOnGround = true; // (ï¿½Ù´Ú¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ñ´Ù°ï¿½ ï¿½ï¿½ï¿½ï¿½)
    m_jumpVelocity = 3.0f;

    // --- 3. CObject ï¿½âº»ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ Render ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½) ---
    // main.cppï¿½ï¿½ ï¿½Ïµï¿½ï¿½Úµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ CObjectï¿½ï¿½ ï¿½âº»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    setScale(vec3(0.3f, 0.3f, 0.3f));
    setRotation(vec3(0, 0.0f, 0.0f)); // Xï¿½ï¿½ 90ï¿½ï¿½ È¸ï¿½ï¿½
    // (translate(0, -1, 0)ï¿½ï¿½ position.yï¿½ï¿½ ï¿½ß·ï¿½ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½)
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

    // --- 1. ï¿½Ìµï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½Å±ï¿½ Player.cpp) ---
    glm::vec3 viewVec = tar - pos;
    viewVec.y = 0.0f;
    viewVec = glm::normalize(viewVec);
    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
    rightVec = glm::normalize(rightVec);

    bool isMoving = false;
    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) {
        position += viewVec * static_cast<float>(dt * 1.0);
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) {
        position += rightVec * static_cast<float>(dt * 1.0);
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) {
        position -= viewVec * static_cast<float>(dt * 1.0);
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
        position -= rightVec * static_cast<float>(dt * 1.0);
        isMoving = true;
    }

    // --- 2. ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½Å±ï¿½ Player.cpp) ---
    // (isJumping ï¿½ï¿½ï¿½ m_isOnGround ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½)
    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD && m_isOnGround) {
        m_isOnGround = false;
        m_velocity.y = m_jumpVelocity;
    }


    // ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ß·ï¿½ ï¿½ï¿½ï¿½ï¿½ (Gravity() ï¿½Ô¼ï¿½ ï¿½ï¿½ï¿½ï¿½)
    ApplyGravity();

    // --- 3. ï¿½Ö´Ï¸ï¿½ï¿½Ì¼ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ Player ï¿½ï¿½ï¿½ï¿½) ---
    glm::vec3 nextPosition = this->position + (m_velocity * (float)dt);

    // --- 5. ï¿½æµ¹ Ã³ï¿½ï¿½ (CSceneï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ê¿ï¿½) ---
    CScene* pCurScene = SceneMgr::Instance()->getScene();
    if (!pCurScene) return;
    // (ï¿½ï¿½ï¿½ï¿½) std::vector<CObject*> collidables = m_pCurrentScene->GetCollidables();

    const vector<CObject*>& mapObjects = pCurScene->GetObjects(GROUP_TYPE::DEFAULT);
    const vector<CObject*>& enemyObjects = pCurScene->GetObjects(GROUP_TYPE::ENEMY);

    glm::vec3 originalPos = this->position;
    bool isGroundedThisFrame = false;

    // Yï¿½ï¿½(ï¿½ß·ï¿½/ï¿½ï¿½ï¿½ï¿½) ï¿½æµ¹ ï¿½ï¿½ï¿½ï¿½ ï¿½Ë»ï¿½
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

    // m_isOnGround ï¿½ï¿½ï¿½ï¿½ È®ï¿½ï¿½
    m_isOnGround = isGroundedThisFrame;

    // Xï¿½ï¿½(ï¿½Â¿ï¿½) ï¿½æµ¹ ï¿½Ë»ï¿½
    this->setPosition(glm::vec3(nextPosition.x, originalPos.y, originalPos.z));
    for (CObject* other : mapObjects) { // ï¿½Ê°ï¿½ Xï¿½ï¿½ ï¿½Ë»ï¿½
        if (this->CheckCollision(*other)) {
            m_velocity.x = 0;
            nextPosition.x = originalPos.x;
            break;
        }
    }
    for (CObject* other : enemyObjects) { // ï¿½ï¿½ï¿½ï¿½ Xï¿½ï¿½ ï¿½Ë»ï¿½
        if (this->CheckCollision(*other)) {
            m_velocity.x = 0;
            nextPosition.x = originalPos.x;
            break;
        }
    }

    // Zï¿½ï¿½(ï¿½Õµï¿½) ï¿½æµ¹ ï¿½Ë»ï¿½
    this->setPosition(glm::vec3(originalPos.x, originalPos.y, nextPosition.z));
    for (CObject* other : mapObjects) { // ï¿½Ê°ï¿½ Zï¿½ï¿½ ï¿½Ë»ï¿½
        if (this->CheckCollision(*other)) {
            m_velocity.z = 0;
            nextPosition.z = originalPos.z;
            break;
        }
    }
    for (CObject* other : enemyObjects) { // ï¿½ï¿½ï¿½ï¿½ Zï¿½ï¿½ ï¿½Ë»ï¿½
        if (this->CheckCollision(*other)) {
            m_velocity.z = 0;
            nextPosition.z = originalPos.z;
            break;
        }
    }

    // ï¿½ï¿½Ä¡ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    this->setPosition(originalPos);

    setPosition(nextPosition); // 11/7 ´ÊÀºÆÐÄ¡ - ½¹¹Î

    if (KeyMgr::Instance()->getKeyState(KEY::E) == KEY_TYPE::TAP) { // 11/11 ´ÊÀºÆÐÄ¡ - ½¹¹Î
        // Ä«¸Þ¶óÀÇ ½Ã¾ß ¹æÇâÀ¸·Î
        CCamera* cam = CameraMgr::Instance()->getMainCamera();
        glm::vec3 camPos = cam->position;
        glm::vec3 camTarget = cam->target;

        glm::vec3 fireOrigin = camPos;
        glm::vec3 fireDirection = glm::normalize(camTarget - camPos);

        C2S_FireAction firePkt = BuildFirePacket(fireOrigin, fireDirection);
        std::cout << "[Fire] FireAction Packet Built! Seq: " << firePkt.msgSeq << std::endl;
    }

    C2S_MovementUpdate movementPkt = BuildMovementPacket();

    if (isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pRunAnim)
            m_pAnimator->PlayAnimation(m_pRunAnim);
    }
    else if (!isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pIdleAnim)
            m_pAnimator->PlayAnimation(m_pIdleAnim);
    }
    // (ï¿½ï¿½ï¿½ï¿½: ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´Ï¸ï¿½ï¿½Ì¼ï¿½ ï¿½ï¿½ï¿½Âµï¿½ ï¿½ß°ï¿½ ï¿½Ê¿ï¿½)
    if (m_pAnimator) {
        m_pAnimator->UpdateAnimation(DT);
    }

    // --- 4. ï¿½ï¿½Æ®ï¿½Ú½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ® (ï¿½Å±ï¿½ Player.cpp) ---
    hitboxCenter = position;
    hitboxSize = scale;


void Player::Render() {
    if (m_pModel) { // [Refactored]
        glUseProgram(CCore::Instance()->shaderProgramID2);
        {
            AnimModel* currModel = m_pModel->GetModel(); // [Refactored]

            // CObjectï¿½ï¿½ P, R, Sï¿½ï¿½ Model Matrix ï¿½ï¿½ï¿½
            glm::mat4 modelMat = glm::mat4(1.0f);
            glm::mat4 T = glm::translate(glm::mat4(1.f), position);
            glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z))));
            glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
            modelMat = T * R * S;

            // (main.cppï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ translate)
            modelMat = glm::translate(modelMat, vec3(0.0f, 0.1f, 0.0f));

            // PVM ï¿½ï¿½ ï¿½ï¿½ï¿½Ì´ï¿½ Uniform ï¿½ï¿½ï¿½ï¿½
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

            // Bone(ï¿½ï¿½) Uniform ï¿½ï¿½ï¿½ï¿½
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices();
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++)
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

            // ï¿½Ø½ï¿½Ã³ Sampler ï¿½ï¿½ï¿½ï¿½
            GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
            GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
            glUniform1i(loc_diffuseSampler, 0);
            glUniform1i(loc_normalSampler, 1);

            // ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
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

    // ï¿½Þ½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    m_movementSeq++;
    pkt.msgSeq = m_movementSeq;

    // PlayerID ï¿½Ó½Ã·ï¿½ 0 Ã³ï¿½ï¿½
    pkt.playerId = 0;

    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡, È¸ï¿½ï¿½, ï¿½Óµï¿½ ï¿½ï¿½ï¿½ï¿½.
    pkt.position.x = position.x;
    pkt.position.y = position.y;
    pkt.position.z = position.z;

    pkt.rotation.x = rotation.x;
    pkt.rotation.y = rotation.y;
    pkt.rotation.z = rotation.z;

    pkt.velocity.x = m_velocity.x;
    pkt.velocity.y = m_velocity.y;
    pkt.velocity.z = m_velocity.z;

    // Å¬ï¿½ï¿½ï¿½Ì¾ï¿½Æ® ï¿½Ã°ï¿½ Å¸ï¿½Ó½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    pkt.clientTime = (float)TimeMgr::Instance()->getCurrTime();

    return pkt;
}

C2S_FireAction Player::BuildFirePacket(const vec3& fireOrigin, const vec3& fireDirection)
{
    C2S_FireAction pkt;

    // ¸Þ½ÃÁö ½ÃÄö½º Áõ°¡ ¹× ¼³Á¤
    m_fireSeq++;
    pkt.msgSeq = m_fireSeq;

    //  PlayerID ÀÓ½Ã·Î 0 Ã³¸®
    pkt.playerId = 0;

    // ÇöÀç À§Ä¡, È¸Àü, ¼Óµµ ¼³Á¤
    pkt.fireOrigin.x = fireOrigin.x;
    pkt.fireOrigin.y = fireOrigin.y;
    pkt.fireOrigin.z = fireOrigin.z;

    pkt.fireDirection.x = fireDirection.x;
    pkt.fireDirection.y = fireDirection.y;
    pkt.fireDirection.z = fireDirection.z;

    // Å¬¶óÀÌ¾ðÆ® ½Ã°£ Å¸ÀÓ½ºÅÆÇÁ ¼³Á¤
    pkt.clientTime = (float)TimeMgr::Instance()->getCurrTime();

    return pkt;
}

void Player::ApplyGravity() {
    double dt = DT;

    m_velocity.y += m_gravity * dt; // ï¿½ß·ï¿½ ï¿½ï¿½ï¿½Óµï¿½ ï¿½ï¿½ï¿½ï¿½
    position.y += m_velocity.y * dt; // yï¿½ï¿½ ï¿½Óµï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ý¿ï¿½

    m_isOnGround = false; // ï¿½âº»ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

    // ï¿½Ù´Ú°ï¿½ ï¿½æµ¹ Ã³ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ ï¿½Ïµï¿½ï¿½Úµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½)
    if (position.x < -2.31563 && position.z < -6.84576) {
        if (position.y <= -0.5f) {
            position.y = -0.5f;
            m_velocity.y = 0.0f;
            m_isOnGround = true; // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
        }
    }
    else if (position.y <= -0.25f) {
        position.y = -0.25f;
        m_velocity.y = 0.0f;
        m_isOnGround = true; // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    }
}
