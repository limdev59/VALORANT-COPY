#include "pch.h"
#include "Player.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "MouseMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"


//float maxJumpHeight = 0.75f; // 최대 점프 높이

Player::Player() {
   model = new AnimModel();

    Animation* walkAnim = new Animation("Models/first2/firstIdle.gltf", (AnimModel*)model);
    animator = new Animator(walkAnim);
}

void Player::Update()
{
    float dt = TimeMgr::Instance()->GetDT();

    // 1. 애니메이터 업데이트 (기존 코드)
    animator->UpdateAnimation(dt);

    // 2. 카메라 방향 벡터 가져오기 (앞, 옆)
    glm::vec3 camFront = CameraMgr::Instance()->getMainCamera()->front;
    glm::vec3 camRight = CameraMgr::Instance()->getMainCamera()->right;
    camFront.y = 0; // 수평 이동만
    camRight.y = 0;
    camFront = glm::normalize(camFront);
    camRight = glm::normalize(camRight);

    float speed = 2.0f * dt; // 이동 속도
    bool isMoving = false;

    // 3. 키 입력 처리 (main.cpp에서 가져온 로직)
    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) {
        position += camFront * speed;
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) {
        position -= camFront * speed;
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) {
        position -= camRight * speed;
        isMoving = true;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
        position += camRight * speed;
        isMoving = true;
    }

    // 4. 애니메이션 상태 변경 (main.cpp에서 가져온 로직)
    // (runAnim, idleAnim은 Player 생성자에서 멤버 변수로 로드해둬야 함)
    // if (isMoving) {
    //     if (animator->GetCurrAnimation() != runAnim)
    //         animator->PlayAnimation(runAnim);
    // }
    // else {
    //     if (animator->GetCurrAnimation() != idleAnim)
    //         animator->PlayAnimation(idleAnim);
    // }

    // 5. 중력 및 객체 업데이트
    Gravity(m_isOnGround);
    CObject::Update(); // (CObject::Update는 현재 비어있지만, 호출)
}
glm::mat3 GetNormalMat(glm::mat4& modelMat)
{
    return glm::mat3(glm::transpose(glm::inverse(modelMat)));
}
void Player::Render() {
    GLuint shaderID = m_shaderProgramID;
    glUseProgram(shaderID);

    // 1. Model Matrix 계산 (CObject의 멤버 변수 사용)
    //    (기존의 하드코딩된 scale/rotate 대신 객체의 상태를 사용해야 합니다)
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    modelMat = glm::rotate(modelMat, glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    modelMat = glm::rotate(modelMat, glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::rotate(modelMat, glm::radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::scale(modelMat, scale);

    // 2. Camera Matrix 계산
    mat4 projMat = CameraMgr::Instance()->getMainCamera()->getProjectionMatrix();
    mat4 view = CameraMgr::Instance()->getMainCamera()->getViewMatrix();
    glm::mat4 PVM = projMat * view * modelMat;
    glm::mat3 normalMat = GetNormalMat(modelMat); // (GetNormalMat 함수가 전역에 있다고 가정)

    // 3. Matrix Uniform 전송
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    glUniformMatrix3fv(glGetUniformLocation(shaderID, "normalMat"), 1, GL_FALSE, glm::value_ptr(normalMat));

    // 4. Bone Uniform 전송 (Player가 자신의 animator를 소유)
    if (animator) { // (animator는 Player의 멤버 변수여야 함)
        const auto& transforms = animator->GetFinalBoneMatrices();
        GLuint finalBonesMatricesLoc = glGetUniformLocation(shaderID, "finalBonesMatrices");
        for (int i = 0; i < transforms.size(); i++)
            glUniformMatrix4fv(finalBonesMatricesLoc + i, 1, false, glm::value_ptr(transforms[i]));
    }

    // 5. Model에게 최종 그리기를 명령
    if (model) { // (model은 CObject로부터 상속받은 IModel*)
        // 모델에게 셰이더 ID를 넘겨주어, 모델이 텍스처 유닛을 설정할 수 있게 함
        model->Render(shaderID);
    }
}

void Player::Gravity(bool isGra) {
    double dt = DT;

    if (isGra) {
        m_velocity.y += m_gravity * dt; // 중력 가속도 적용
        position.y += m_velocity.y * dt; // y축 속도를 위치에 반영

        // 바닥과 충돌 처리
        if (position.x < -2.31563 && position.z < -6.84576) {
            if (position.y <= -0.5f) {
                position.y = -0.5f;
                m_velocity.y = 0.0f;
                m_isJumping = false; // 점프 종료
            }
        }
        else if (position.y <= -0.25f) {
            position.y = -0.25f;
            m_velocity.y = 0.0f;
            m_isJumping = false; // 점프 종료
        }
        

        if (model) {
            model->Update(position, rotation, scale);
        }

        hitboxCenter = position;
        hitboxSize *= scale;
    }
    else {
        m_velocity.y = 0.0f;
    }
}