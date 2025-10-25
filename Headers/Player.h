#pragma once
#include "CObject.h"
#include "AnimModel.h"

// Player.h
class Player : public CObject {
private:
    float   m_gravity = -9.81f;      // 중력 값
    vec3    m_velocity = vec3(0.0f);  // 속도
    bool    m_isOnGround = false;     // 바닥과 충돌 여부
    bool    m_isJumping = false; // 점프 상태를 나타냄
    float   m_jumpVelocity = 1.0f; // 초기 점프 속도

    
    Animator* animator;

public:
    Player();
    virtual void Update() override;
    void Render() override;
    void Gravity(bool);  // 중력 처리 함수
    bool CheckCollisionWithGround(); // 바닥과의 충돌 체크 함수
    // 기타 멤버들...
};

