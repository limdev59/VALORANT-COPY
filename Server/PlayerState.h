#pragma once

#include "PacketDefs.h"

#include <cstdio>

/**
 * @class PlayerState
 * @brief 개별 플레이어의 상태 데이터.
 *
 */
class PlayerState
{
public:
    PlayerState(PlayerID id);

    PlayerID GetPlayerID() const { return m_PlayerID; }

    /**
     * @brief 클라이언트의 이동 입력을 받아 서버에 반영
     * (2주차 작업)
     */
    void ApplyMovementFromClient(const C2S_MovementUpdate& pkt, float serverNow);

    /**
     * @brief 클라이언트의 발사 입력을 받아 서버에서 처리
     * (3주차 작업)
     */
    void ApplyFireFromClient(const C2S_FireAction& pkt);

    /**
     * @brief 플레이어에게 데미지를 적용
     */
    void ApplyDamage(int dmg);

    /**
     * @brief PlayerState의 현재 상태를 패킷용 PlayerSnapshot으로 변환
     * (11/5 작업) 김도윤
     */
    PlayerSnapshot ToSnapshot() const;

private:
    PlayerID	m_PlayerID;
    Vec3		m_Position;
    Vec3        m_Rotation;
    Vec3		m_Velocity;
    uint8_t     m_InputKeys;
    bool        m_IsOnGround; 
    float		m_Health;
};

