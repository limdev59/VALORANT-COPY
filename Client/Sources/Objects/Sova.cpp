#include "pch.h"
#include "Sova.h"
#include "CCore.h"
#include "TimeMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"

Sova::Sova()
    : CObject() {

    string modelPath = "first2";
    m_pModel = new IModel<AnimModel>(modelPath);

    AnimModel* currModel = m_pModel->GetModel(); // ·¡ÆÛ¿¡¼­ ½ÇÁ¦ ¸ðµ¨ Æ÷ÀÎÅÍ °¡Á®¿À±â

    m_pIdleAnim = new Animation("Models/first2/firstIdle.gltf", currModel);
    m_pAnimator = new Animator(m_pIdleAnim);

    m_gravity = -9.81f;
    m_velocity = vec3(0.0f);
    m_isOnGround = true; // (¹Ù´Ú¿¡¼­ ½ÃÀÛÇÑ´Ù°í °¡Á¤)
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
    hitboxCenter = glm::vec3(0.0f);
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

            // 3. [Á¦¿Ü] Player Àü¿ë ¸ðµ¨ ¿øÁ¡ º¸Á¤ ÄÚµå´Â ÀÌ½ÄÇÏÁö ¾ÊÀ½
            // modelMat = glm::translate(modelMat, vec3(0.0f, -1.0f, 0.0f));

            // 4. PVM ¹× ¼ÎÀÌ´õ Uniform ¼³Á¤ (Player¿Í µ¿ÀÏ)
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

            // 5. Bone(»À) Uniform ¼³Á¤ (Sova ÀÚ½ÅÀÇ m_pAnimator »ç¿ë)
            const auto& transforms = m_pAnimator->GetFinalBoneMatrices(); // [¼öÁ¤]
            GLuint finalBonesMatricesLoc = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "finalBonesMatrices");
            for (int i = 0; i < transforms.size(); i++)
                glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));

            // 6. ÅØ½ºÃ³ Sampler ¼³Á¤ (Player¿Í µ¿ÀÏ)
            GLint loc_diffuseSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "diffuseTexture");
            GLint loc_normalSampler = glGetUniformLocation(CCore::Instance()->shaderProgramID2, "normalMap");
            glUniform1i(loc_diffuseSampler, 0);
            glUniform1i(loc_normalSampler, 1);

            // 7. ¸ðµ¨ ·»´õ¸µ
            currModel->Render();

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
                std::cout << "Sova Render error : " << error << std::endl; // µð¹ö±ë¿ë
        }
    }
}

void Sova::ApplyGravity() {
    double dt = DT;

    m_velocity.y += m_gravity * dt; // Áß·Â °¡¼Óµµ Àû¿ë
    position.y += m_velocity.y * dt; // yÃà ¼Óµµ¸¦ À§Ä¡¿¡ ¹Ý¿µ

    m_isOnGround = false; // ±âº»°ªÀº °øÁß

    // ¹Ù´Ú°ú Ãæµ¹ Ã³¸® (±âÁ¸ ÇÏµåÄÚµùµÈ ·ÎÁ÷ À¯Áö)
    if (position.x < -2.31563 && position.z < -6.84576) {
        if (position.y <= -0.5f) {
            position.y = -0.5f;
            m_velocity.y = 0.0f;
            m_isOnGround = true; // Á¡ÇÁ Á¾·á
        }
    }
    else if (position.y <= -0.25f) {
        position.y = -0.25f;
        m_velocity.y = 0.0f;
        m_isOnGround = true; // Á¡ÇÁ Á¾·á
    }
}