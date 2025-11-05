#include "WorldState.h"

WorldState::WorldState()
	: m_pInputQueue(nullptr)
	, m_pOutputQueue(nullptr)
{
}

WorldState::~WorldState()
{
    for (auto& pair : m_PlayerStates)
    {
        delete pair.second; // PlayerState 객체 메모리 해제
    }
    m_PlayerStates.clear();
}

void WorldState::Initialize(PacketQueue* inQ, ByteQueue* outQ)
{
    m_pInputQueue = inQ;
    m_pOutputQueue = outQ;
}

void WorldState::Tick(float nowSec)
{
    // 입력 큐 처리
    WorldEvent event;
    // 큐가 빌 때까지 모든 이벤트를 처리
    while (m_pInputQueue->TryPop(event))
    {
        switch (event.type)
        {
        case E_PlayerJoined:
            OnPlayerJoined(event.playerID);
            break;
        case E_PlayerLeft:
            OnPlayerLeft(event.playerID);
            break;
        case E_Packet_Movement:
            OnMovement(event.playerID, event.movement, nowSec);
            break;
        case E_Packet_Fire:
            OnFire(event.playerID, event.fire);
            break;
        }
    }

    // 게임 상태 업데이트

    // 스냅샷 생성
    S2C_SnapshotState snapshotPkt;
    BuildSnapshotAll(nowSec, snapshotPkt);

    // 스냅샷 출력 큐에 삽입
    if (!snapshotPkt.snapshots.empty())
    {
        // TODO: snapshotPkt를 std::vector<uint8_t>로 직렬화(Serialize)해야함
    }
}

// --- 이벤트 핸들러 구현 ---
void WorldState::OnPlayerJoined(PlayerID pid)
{
    if (m_PlayerStates.find(pid) == m_PlayerStates.end())
    {
        m_PlayerStates[pid] = new PlayerState(pid);
        printf("[WorldState] Player %u added.\n", pid);
    }
}

void WorldState::OnPlayerLeft(PlayerID pid)
{
    auto it = m_PlayerStates.find(pid);
    if (it != m_PlayerStates.end())
    {
        delete it->second;
        m_PlayerStates.erase(it);
        printf("[WorldState] Player %u removed.\n", pid);
    }
}

void WorldState::OnMovement(PlayerID pid, const C2S_MovementUpdate& pkt, float nowSec)
{
    auto it = m_PlayerStates.find(pid);
    if (it != m_PlayerStates.end())
    {
        it->second->ApplyMovementFromClient(pkt, nowSec);
    }
}

void WorldState::OnFire(PlayerID pid, const C2S_FireAction& pkt)
{
    auto it = m_PlayerStates.find(pid);
    if (it != m_PlayerStates.end())
    {
        it->second->ApplyFireFromClient(pkt);
    }
}

// (3주차 작업)
void WorldState::BuildSnapshotAll(float nowSec, S2C_SnapshotState& outPkt)
{
}