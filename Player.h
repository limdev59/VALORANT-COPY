#pragma once
#include "CObject.h"
#include "AnimModel.h"

// Player.h
class Player : public CObject {
private:
    float gravity = -9.81f;      // 중력 값
    vec3 velocity = vec3(0.0f);  // 속도
    bool isOnGround = false;     // 바닥과 충돌 여부
    AnimModel* model;
public:
	Player();
    virtual void Update() override;
    virtual void Render() override;
    void Gravity(bool);  // 중력 처리 함수
    bool CheckCollisionWithGround(); // 바닥과의 충돌 체크 함수
    // 기타 멤버들...
};

