#include "pch.h"
#include "Player.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "MouseMgr.h"


void Player::Update()
{
    double dt = DT; // DT 값을 가져옵니다.
    
    if (KeyMgr::Instance()->getKeyState(KEY::W) == KEY_TYPE::HOLD) {
        position.x -= 1.0 * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::A) == KEY_TYPE::HOLD) {
        position.z += 1.0 * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::S) == KEY_TYPE::HOLD) {
        position.x += 1.0 * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::D) == KEY_TYPE::HOLD) {
        position.z -= 1.0 * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::Q) == KEY_TYPE::HOLD) {
        position.y += 2.0 * dt;
    }
    if (KeyMgr::Instance()->getKeyState(KEY::E) == KEY_TYPE::HOLD) {
        position.y -= 1.0 * dt;
    }
    if (model) {
        model->Update(position, rotation, scale);
    }
    hitboxCenter = position;
    hitboxSize *= scale;
}

void Player::Gravity(bool isGra) {
    double dt = DT;
    if (isGra){
        velocity.y += gravity * dt;
        position.y += velocity.y * dt;  // y축에 속도를 더함

        if (model) {
            model->Update(position, rotation, scale);
        }
        hitboxCenter = position;
        hitboxSize *= scale;
    }
    else {
        velocity.y = 0.0f;
    }
}
