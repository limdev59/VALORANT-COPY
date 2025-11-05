#pragma once

#include "PacketDefs.h"

/**
 * @enum WorldEventType
 * @brief PacketDispatcher가 WorldState 스레드에게 보낼 이벤트의 종류 정의
 * (패킷 외에 PlayerJoined, PlayerLeft 보고 포함)
 * 김도윤
 */
enum WorldEventType
{
    E_PlayerJoined      = 1,  // 새 플레이어 접속 (내부)
    E_PlayerLeft        = 2,  // 플레이어 접속 종료 (내부)
    E_Packet_Movement   = 3,  // C2S_MOVEMENT_UPDATE 수신 
    E_Packet_Fire       = 4   // C2S_FIRE_ACTION 수신
};

/**
 * @struct WorldEvent
 * @brief PacketQueue에 저장될 실제 데이터.
 * WorldState는 이 이벤트를 큐에서 꺼내 처리
 * 김도윤
 */
struct WorldEvent
{
    WorldEventType  type;
    PlayerID        playerID;

    // type == E_Packet_Movement 일 때 사용
    C2S_MovementUpdate movement;

    // type == E_Packet_Fire 일 때 사용
    C2S_FireAction fire;
};