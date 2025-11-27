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
    // TODO: snapshotPkt를 std::vector<uint8_t>로 직렬화(Serialize)해야함
    if (!snapshotPkt.snapshots.empty())
    {
        m_SnapshotHistory.push_back(snapshotPkt);

        // 버퍼 크기 관리 (오래된 데이터 삭제)
        while (m_SnapshotHistory.size() > MAX_SNAPSHOT_HISTORY_SIZE)
        {
            m_SnapshotHistory.pop_front();
        }
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
	// 총을 쏜 플레이어 상태 업데이트
    auto it = m_PlayerStates.find(pid);
    if (it != m_PlayerStates.end())
    {
        it->second->ApplyFireFromClient(pkt);
    }

    // 총을 맞은 플레이어 상태 업데이트
    if (pkt.hitPlayerID != -1)
    {
        // 자기 자신을 쏘는 경우 무시
        if (pkt.hitPlayerID == pid) return;

        // 맞은 사람 찾기
        auto itVictim = m_PlayerStates.find(pkt.hitPlayerID);
        if (itVictim != m_PlayerStates.end())
        {
            // 맞은 사람에게 데미지 적용
            itVictim->second->ApplyDamage(20);
        }
    }
}

// 김도윤
void WorldState::BuildSnapshotAll(float nowSec, S2C_SnapshotState& outPkt)
{
    outPkt.type = MsgType::S2C_SNAPSHOT_STATE;

    for (auto& pair : m_PlayerStates)
    {
        PlayerState* pState = pair.second;

        if (pState != nullptr)
        {
            // 1. PlayerState 스냅샷 정보 생성
            PlayerSnapshot snapshot = pState->ToSnapshot();

            // 2. 서버 시간 설정
            snapshot.serverTime = nowSec;

            // 3. 패킷 벡터에 추가
            outPkt.snapshots.push_back(snapshot);
        }
    }
}