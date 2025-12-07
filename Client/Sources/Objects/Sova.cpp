#include "pch.h"
#include "Sova.h"
#include "CCore.h"
#include "TimeMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"

Sova::Sova()
    : CObject() {

    string modelName = "jettSimple";
    string pathName = "jettSimple";
    m_pModel = new IModel<AnimModel>(modelName, pathName);

    AnimModel* currModel = m_pModel->GetModel(); // 모델에서 본 정보를 가져옴

    m_pIdleAnim = new Animation("Animations/jettSimple/jettSimple_aim.gltf", currModel);
    m_pWalkFrontAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkN.gltf", currModel);
    m_pWalkFrontLeftAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkNW.gltf", currModel);
    m_pWalkFrontRightAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkNE.gltf", currModel);
    m_pWalkRightAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkE.gltf", currModel);
    m_pWalkLeftAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkW.gltf", currModel);
    m_pWalkBackLeftAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkSW.gltf", currModel);
    m_pWalkBackRightAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkSE.gltf", currModel);
    m_pWalkBackAnim = new Animation("Animations/jettSimple/jettSimple_aim_walkS.gltf", currModel);
    m_pAnimator = new Animator(m_pIdleAnim);

    m_gravity = -9.81f;
    m_velocity = vec3(0.0f);
    m_isOnGround = true; // (바닥에서 시작한다고 가정)
    m_jumpVelocity = 3.0f;

    setScale(vec3(0.3f, 0.3f, 0.3f));
    setRotation(vec3(0, 0.0f, 0.0f));
}

Sova::~Sova() {
    if (m_pModel) delete m_pModel;
    if (m_pAnimator) delete m_pAnimator;
    if (m_pIdleAnim) delete m_pIdleAnim;
}

void Sova::Update() {
    ApplyGravity();
    if (m_pAnimator) {
        m_pAnimator->PlayAnimation(m_pIdleAnim);
        m_pAnimator->UpdateAnimation(DT);
    }
    hitboxCenter = position;
    hitboxSize = scale;
}

void Sova::Render() {
    if (m_pModel) {
        glUseProgram(CCore::Instance()->shaderProgramID2);
        {
            AnimModel* currModel = m_pModel->GetModel();

            glm::mat4 modelMat = glm::mat4(1.0f);
            glm::mat4 T = glm::translate(glm::mat4(1.f), position);
            glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z))));
            glm::mat4 S = glm::scale(glm::mat4(1.f), scale);
            modelMat = T * R * S;

            // 3. [제외] Player 전용 모델 원점 보정 코드는 이식하지 않음
            // modelMat = glm::translate(modelMat, vec3(0.0f, -1.0f, 0.0f));

            // 4. PVM 및 셰이더 Uniform 설정 (Player와 동일)
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

            // 5. Bone(뼈) Uniform 설정 (Sova 자신의 m_pAnimator 사용)
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices(); // [수정]
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++)
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

            // 6. 텍스처 Sampler 설정 (Player와 동일)
            GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
            GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
            glUniform1i(loc_diffuseSampler, 0);
            glUniform1i(loc_normalSampler, 1);

            // 7. 모델 렌더링
            currModel->Render();

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
                std::cout << "Sova Render error : " << error << std::endl; // 디버깅용
        }
    }
}

void Sova::ApplyGravity() {
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