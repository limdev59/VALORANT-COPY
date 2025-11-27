#pragma once

#include "PacketDefs.h"   
#include "QueueTypes.h"   
#include "WorldEvent.h"  
#include "PlayerState.h"
#include <map>

/**
 * @class WorldState
 * @brief 모든 PlayerState를 소유/관리하고 큐의 이벤트를 처리
 * 김도윤
 */
class WorldState
{
public:
    WorldState();
    ~WorldState();

    /**
     * @brief 스레드 간 통신 큐 연결
     */
    void Initialize(PacketQueue* inQ, ByteQueue* outQ);

    /**
     * @brief 게임 로직의 메인 루프. 입력 큐를 처리하고, 스냅샷을 출력 큐에 보냄
     */
    void Tick(float nowSec);
    /**
     * @brief 과거 스냅샷 버퍼를 반환하는 함수 - 슝민
     */
    const std::deque<S2C_SnapshotState>& GetLastSnapshots() const { return m_SnapshotHistory; }

private:
    // --- 이벤트 처리 함수 ---
    void OnPlayerJoined(PlayerID pid);
    void OnPlayerLeft(PlayerID pid);
    void OnMovement(PlayerID pid, const C2S_MovementUpdate& pkt, float nowSec);
    void OnFire(PlayerID pid, const C2S_FireAction& pkt);

    /**
     * @brief 모든 PlayerState의 스냅샷 생성
     */
    void BuildSnapshotAll(float nowSec, S2C_SnapshotState& outPkt);

private:
    PacketQueue* m_pInputQueue;  // NetworkIO -> WorldState
    ByteQueue* m_pOutputQueue; // WorldState -> NetworkIO

    // 게임에 존재하는 모든 플레이어의 상태
    std::map<PlayerID, PlayerState*> m_PlayerStates;

    // 스냅샷 히스토리 버퍼 (최근 N개의 스냅샷 저장)
    std::deque<S2C_SnapshotState> m_SnapshotHistory;

    const size_t MAX_SNAPSHOT_HISTORY_SIZE = 60;
};