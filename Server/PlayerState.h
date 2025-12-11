#pragma once

#include "PacketDefs.h"

#include <cstdio>

/**
 * @class PlayerState
 * @brief Player state on server.
 */
class PlayerState
{
public:
    PlayerState(PlayerID id);

    PlayerID GetPlayerID() const { return m_PlayerID; }
    bool IsAlive() const { return m_IsAlive; }

    /**
     * @brief Applies movement update from client.
     * (2 단계)
     */
    void ApplyMovementFromClient(const C2S_MovementUpdate& pkt, float serverNow);

    /**
     * @brief Applies fire update from client.
     * (3 단계)
     */
    void ApplyFireFromClient(const C2S_FireAction& pkt);

    /**
     * @brief Applies damage to this player.
     */
    void ApplyDamage(int dmg, float nowSec);

    /**
     * @brief Updates timers such as respawn countdowns.
     */
    void Update(float nowSec);

    /**
     * @brief Converts PlayerState to PlayerSnapshot.
     * (11/5 단계) 제공
     */
    PlayerSnapshot ToSnapshot() const;

private:
    void Respawn();
    Vec3 GetRandomSpawnPosition() const;

private:
    PlayerID    m_PlayerID;
    Vec3        m_Position;
    Vec3        m_Rotation;
    Vec3        m_Velocity;
    uint8_t     m_InputKeys;
    bool        m_IsOnGround;
    float       m_Health;
    bool        m_IsAlive;

    float       m_NextRespawnTime;

    static constexpr float kMaxHealth = 150.0f;
    static constexpr float kRespawnDelay = 3.0f;
};
