#pragma once
#include "CObject.h"
#include "TimeMgr.h" // DT
#include "KeyMgr.h"  // KeyMgr
#include "IModel.h"  // ø  
#include "ClientNetwork.h" // C2S_MovementUpdate, C2S_FireAction

//  
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
    
    Animation* m_pWalkFrontAnim;
    Animation* m_pWalkFrontLeftAnim;
    Animation* m_pWalkFrontRightAnim;
    Animation* m_pWalkRightAnim;
    Animation* m_pWalkLeftAnim;
    Animation* m_pWalkBackLeftAnim;
    Animation* m_pWalkBackRightAnim;
    Animation* m_pWalkBackAnim;

    // --- Physics Members (from existing Player.h) ---
    float   m_gravity = -9.81f;
    vec3    m_velocity = vec3(0.0f);
    bool    m_isOnGround = false;
    bool    m_isMoving = false;
    float   m_jumpVelocity = 2.5f;
    int     m_health = 150;
    bool    m_isDead = false;
    
    
    float   m_radius = 0.03f; 
    float   m_height = 0.23f;
    bool    m_bShowHitbox = false;
    
    bool    m_bThirdPerson = false;

    uint32_t m_movementSeq = 0;
    uint32_t m_fireSeq = 0;
    
    bool      m_bShowFireRay = false;
    float     m_fireRayTimer = 0.0f;
    glm::vec3 m_fireRayStart = glm::vec3(0.0f);
    glm::vec3 m_fireRayEnd = glm::vec3(0.0f);
    
public:
    Player();
    virtual ~Player();

    virtual void Update() override;
    virtual void Render() override;

    void RenderHitbox();
    void RenderFireRay();

    C2S_MovementUpdate BuildMovementPacket();  
    C2S_FireAction BuildFirePacket(const vec3& fireOrigin, const vec3& fireDirection, PlayerID targetID);
    void ApplyGravity();
    
    void CheckFloorCollision(const std::vector<glm::vec3>& mapTriangles);
    void ResolveWallCollision(const std::vector<glm::vec3>& mapTriangles, glm::vec3& nextPos);
    void PreventTunneling(const std::vector<glm::vec3>& mapTriangles, glm::vec3& moveVec);

    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    bool IsDead() const { return m_isDead; }

    void ApplyServerState(const PlayerSnapshot& snap);

};