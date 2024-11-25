#include "pch.h"
#include "Player.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"


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
    if (model) {
        model->Update(position, rotation, scale);
    }
    hitboxCenter = position;
    hitboxSize *= scale;
}



