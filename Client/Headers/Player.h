#pragma once
#include "CObject.h"
#include "TimeMgr.h" // DT
#include "KeyMgr.h"  // KeyMgr
#include "IModel.h"  // 템플릿 래퍼 포함
#include "ClientNetwork.h" // C2S_MovementUpdate, C2S_FireAction

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
    int     m_health = 150;
    
    float   m_radius = 0.02f; 
    float   m_height = 0.06f;
    bool    m_bShowHitbox = false;
    
    bool    m_bThirdPerson = false;

    uint32_t m_movementSeq = 0;
    uint32_t m_fireSeq = 0;
public:
    Player();
    virtual ~Player();

    virtual void Update() override;
    virtual void Render() override;

    void RenderHitbox();

    C2S_MovementUpdate BuildMovementPacket();  
    C2S_FireAction BuildFirePacket(const vec3& fireOrigin, const vec3& fireDirection, PlayerID targetID);
    void ApplyGravity();
    
    void CheckFloorCollision(const std::vector<glm::vec3>& mapTriangles);
    void ResolveWallCollision(const std::vector<glm::vec3>& mapTriangles, glm::vec3& nextPos);
    void PreventTunneling(const std::vector<glm::vec3>& mapTriangles, glm::vec3& moveVec);

    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    glm::vec3 GetVelocity() const { return m_velocity; }
    bool IsOnGround() const { return m_isOnGround; }
};