#pragma once
#include "CObject.h"
#include "IModel.h"

// 전방 선언
class AnimModel;
class Animator;
class Animation;
template <typename T>
class IModel;

class Enemy : public CObject {
protected:
    // --- Model Wrapper (애니메이션 모델로 변경) ---
    IModel<AnimModel>* m_pModel;

    // --- Animation Members (Player와 동일 구성) ---
    Animator* m_pAnimator;
    Animation* m_pIdleAnim;

    Animation* m_pWalkFrontAnim;
    Animation* m_pWalkFrontLeftAnim;
    Animation* m_pWalkFrontRightAnim;
    Animation* m_pWalkRightAnim;
    Animation* m_pWalkLeftAnim;
    Animation* m_pWalkBackLeftAnim;
    Animation* m_pWalkBackRightAnim;
    Animation* m_pWalkBackAnim;

public:
    Enemy();
    virtual ~Enemy();

    virtual void Update() override;
    virtual void Render() override;

    void TakeDamage(int damage);
    void OnDeath();

    // 외부(네트워크 핸들러 등)에서 애니메이션 상태를 설정할 수 있도록 접근자나 함수가 필요할 수 있음
    Animator* GetAnimator() const { return m_pAnimator; }
    
    // 필요 시 애니메이션 포인터들에 접근할 수 있게 getter 제공 (상태 동기화용)
    Animation* GetIdleAnim() const { return m_pIdleAnim; }
    Animation* GetWalkFrontAnim() const { return m_pWalkFrontAnim; }
    Animation* GetWalkFrontLeftAnim() const { return m_pWalkFrontLeftAnim; }
    Animation* GetWalkFrontRightAnim() const { return m_pWalkFrontRightAnim; }
    Animation* GetWalkRightAnim() const { return m_pWalkRightAnim; }
    Animation* GetWalkLeftAnim() const { return m_pWalkLeftAnim; }
    Animation* GetWalkBackLeftAnim() const { return m_pWalkBackLeftAnim; }
    Animation* GetWalkBackRightAnim() const { return m_pWalkBackRightAnim; }
    Animation* GetWalkBackAnim() const { return m_pWalkBackAnim; }

private:
    int hp = 100;
    int atk = 30;
};