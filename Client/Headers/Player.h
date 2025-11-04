#pragma once
#include "CObject.h"
#include "TimeMgr.h" // DT
#include "KeyMgr.h"  // KeyMgr
#include "IModel.h"  // 템플릿 래퍼 포함

// 전방 선언
class AnimModel;
class Animator;
class Animation;
template <typename T>
class IModel;

class Player : public CObject {
protected:
    // --- Model Wrapper ---
    IModel<AnimModel>* m_pModel;

    // --- Animation Members (from main.cpp logic) ---
    Animator* m_pAnimator;

    Animation* m_pIdleAnim;
    Animation* m_pRunAnim;

    // --- Physics Members (from existing Player.h) ---
    float   m_gravity = -9.81f;
    vec3    m_velocity = vec3(0.0f);
    bool    m_isOnGround = false;
    float   m_jumpVelocity = 2.0f;

public:
    Player();
    virtual ~Player();

    virtual void Update() override;
    virtual void Render() override;

    // --- Physics Methods (from existing Player.h/Player.cpp) ---
    void ApplyGravity(); // [변경] Gravity(bool) -> ApplyGravity()

};