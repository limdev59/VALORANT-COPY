#include "pch.h"
#include "Player.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "MouseMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"

bool isJumping = false; // 점프 상태를 나타냄
float jumpVelocity = 3.0f; // 초기 점프 속도
//float maxJumpHeight = 0.75f; // 최대 점프 높이

Player::Player()
	: CObject(), model(nullptr)
{
    velocity = glm::vec3(0.0f);
    gravity = -9.8f;
	rotation = glm::vec3(0.0f);
    scale = glm::vec3(0.1f);
}

void Player::Update()
{
    double dt = DT; // DT 값을 가져옵니다.
    CCamera* cam = CameraMgr::Instance()->getMainCamera();
    glm::vec3 tar = cam->target;
    glm::vec3 pos = cam->position;


    glm::vec3 viewVec = tar - pos;
    viewVec.y = 0.0f; // Y 성분 제거
    if (glm::length(viewVec) > 0.0001f)
    {
        viewVec = glm::normalize(viewVec);
    }
    else
    {
        viewVec = glm::vec3(0.0f);
    }

    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
    if (glm::length(rightVec) > 0.0001f)
    {
        rightVec = glm::normalize(rightVec);
    }
    else
    {
        rightVec = glm::vec3(0.0f); // A/D 이동 방지
    }

    // 플레이어 이동 처리
    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) {
        position += viewVec * static_cast<float>(dt * 1.0);
    }
    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) {
        position += rightVec * static_cast<float>(dt*1.0);
    }
    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) {
        position -= viewVec * static_cast<float>(dt*1.0);
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
        position -= rightVec * static_cast<float>(dt * 1.0);
    }
  
    if (KeyMgr::Instance()->getKeyState(KEY::SPACE) == KEY_TYPE::HOLD && !isJumping) {
        isJumping = true;
        velocity.y = jumpVelocity; // 초기 점프 속도를 적용
    }
    // 모델 업데이트
    /*if (model) {
        model->UpdateTransform(position, rotation, scale);
    }
    hitboxCenter = position;
    hitboxSize = scale;*/
}

void Player::Render() {
    if (model) {
        model->Render();
    }
}

void Player::Gravity(bool isGra) {
    double dt = DT;

    if (isGra) {
        velocity.y += gravity * dt; // 중력 가속도 적용
        position.y += velocity.y * dt; // y축 속도를 위치에 반영

        // 바닥과 충돌 처리
        if (position.x < -2.31563 && position.z < -6.84576) {
            if (position.y <= -0.5f) {
                position.y = -0.5f;
                velocity.y = 0.0f;
                isJumping = false; // 점프 종료
            }
        }
        else if (position.y <= -0.25f) {
            position.y = -0.25f;
            velocity.y = 0.0f;
            isJumping = false; // 점프 종료
        }
        

        if (model) {
           model->UpdateTransform(position, rotation, scale);
        }

        hitboxCenter = position;
        hitboxSize = scale;
    }
    else {
        velocity.y = 0.0f;
        if (model) {
            model->UpdateTransform(position, rotation, scale);
        }
        hitboxCenter = position;
        hitboxSize = scale;
    }
}