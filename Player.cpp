#include "pch.h"
#include "Player.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "MouseMgr.h"
#include "CameraMgr.h"
#include "CCamera.h"


void Player::Update()
{
    double dt = DT; // DT ���� �����ɴϴ�.
    CCamera* cam = CameraMgr::Instance()->getMainCamera();
    glm::vec3 tar = cam->target;
    glm::vec3 pos = cam->position;


    glm::vec3 viewVec = tar - pos;
    viewVec.y = 0.0f; // Y ���� ����
    viewVec = glm::normalize(viewVec);

    glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), viewVec);
    rightVec = glm::normalize(rightVec);

    // �÷��̾� �̵� ó��
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
  

    // ��Ʈ�ڽ� ������Ʈ    
    hitboxCenter = position;
    hitboxSize = scale;
}

void Player::Render() {
    if (model) {
        model->Render();
    }
}

void Player::Gravity(bool isGra) {
    double dt = DT;
    if (isGra){
        velocity.y += gravity * dt;
        position.y += velocity.y * dt;  // y�࿡ �ӵ��� ����

        if (position.x < -2.31563 && position.z < -6.84576) {
            if (position.y <= -0.5f) {
                position.y = -0.5f;
            }

        }
        else if (position.y <= -0.25f) {
            
            position.y = -0.25f;
        }
        
        hitboxCenter = position;
        hitboxSize *= scale;
    }

    else {
        velocity.y = 0.0f;
    }
}