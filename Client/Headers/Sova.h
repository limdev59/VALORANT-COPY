#pragma once
#include "CObject.h"
#include "IModel.h"

class AnimModel;
class Animator;
class Animation;
template <typename T>
class IModel;

class Sova : public CObject {
protected:
    IModel<AnimModel>* m_pModel;

    Animator* m_pAnimator; 

    Animation* m_pIdleAnim;

    float   m_gravity = -9.81f;
    vec3    m_velocity = vec3(0.0f);
    bool    m_isOnGround = false;
    float   m_jumpVelocity = 2.0f;

public:
    Sova();
    virtual ~Sova();

    virtual void Update() override;
    virtual void Render() override;
    void ApplyGravity();
};
