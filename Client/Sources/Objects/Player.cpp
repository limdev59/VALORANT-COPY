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

    // --- 1. Model / Animation 로딩 (이전 Player 로직) ---
    string modelPath = "first2";
    m_pModel = new IModel<AnimModel>(modelPath); // [Refactored]

    AnimModel* currModel = m_pModel->GetModel(); // 래퍼에서 실제 모델 포인터 가져오기

    m_pIdleAnim = new Animation("Models/first2/firstIdle.gltf", currModel);
    m_pRunAnim = new Animation("Models/first2/first.gltf", currModel);
    
    m_pAnimator = new Animator(m_pIdleAnim);

    // --- 2. Physics 변수 초기화 (신규 Player 로직) ---
    m_gravity = -9.81f;
    m_velocity = vec3(0.0f);
    m_isOnGround = true; // (바닥에서 시작한다고 가정)
    m_jumpVelocity = 3.0f;

    // --- 3. CObject 기본값 설정 (이전 Render 로직에서 추출) ---
    // main.cpp의 하드코딩된 값을 CObject의 기본값으로 설정
    setScale(vec3(0.3f, 0.3f, 0.3f));
    setRotation(vec3(0, 0.0f, 0.0f)); // X축 90도 회전
    // (translate(0, -1, 0)은 position.y와 중력으로 처리)
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

    // --- 1. 이동 로직 (신규 Player.cpp) ---
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

    // --- 2. 점프 및 물리 로직 (신규 Player.cpp) ---
    // (isJumping 대신 m_isOnGround 멤버 변수 사용)
    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD && m_isOnGround) {
        m_isOnGround = false;
        m_velocity.y = m_jumpVelocity;
    }

    // 매 프레임 중력 적용 (Gravity() 함수 내용)
    ApplyGravity();

    // --- 3. 애니메이션 상태 변경 (이전 Player 로직) ---
    if (isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pRunAnim)
            m_pAnimator->PlayAnimation(m_pRunAnim);
    }
    else if (!isMoving && m_isOnGround) {
        if (m_pAnimator->GetCurrAnimation() != m_pIdleAnim)
            m_pAnimator->PlayAnimation(m_pIdleAnim);
    }
    // (참고: 점프 애니메이션 상태도 추가 필요)
    if (m_pAnimator) {
        m_pAnimator->UpdateAnimation(DT);
    }

    // --- 4. 히트박스 업데이트 (신규 Player.cpp) ---
    hitboxCenter = position;
    hitboxSize = scale;

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

            // (main.cpp에서 가져온 모델 원점 보정용 translate)
            modelMat = glm::translate(modelMat, vec3(0.0f, 0.1f, 0.0f));

            // PVM 및 셰이더 Uniform 설정
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

            // Bone(뼈) Uniform 설정
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices();
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++)
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

            // 텍스처 Sampler 설정
            GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
            GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
            glUniform1i(loc_diffuseSampler, 0);
            glUniform1i(loc_normalSampler, 1);

            // 모델 렌더링
            currModel->Render();

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
                std::cout << "error : " << error << std::endl;
        }
    }
}

void Player::ApplyGravity() {
    double dt = DT;

    m_velocity.y += m_gravity * dt; // 중력 가속도 적용
    position.y += m_velocity.y * dt; // y축 속도를 위치에 반영

    m_isOnGround = false; // 기본값은 공중

    // 바닥과 충돌 처리 (기존 하드코딩된 로직 유지)
    if (position.x < -2.31563 && position.z < -6.84576) {
        if (position.y <= -0.5f) {
            position.y = -0.5f;
            m_velocity.y = 0.0f;
            m_isOnGround = true; // 점프 종료
        }
    }
    else if (position.y <= -0.25f) {
        position.y = -0.25f;
        m_velocity.y = 0.0f;
        m_isOnGround = true; // 점프 종료
    }
}
