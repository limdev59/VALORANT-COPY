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
        delete pair.second; // PlayerState ��ü �޸� ����
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
    // 큐에 담긴 모든 이벤트를 처리
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
            OnFire(event.playerID, event.fire, nowSec);
            break;
        }
    }

    for (auto& pair : m_PlayerStates)
    {
        if (pair.second)
        {
            pair.second->Update(nowSec);
        }
    }

    // 표준 상태 사진 생성

    // 파라미터 추적
    S2C_SnapshotState snapshotPkt;

    int playerCount = BuildSnapshotAll(nowSec, snapshotPkt);

    // 생성한 스냅샷을 출력 큐에 전달
    if (playerCount > 0)
    {
        // 1. 헤더와 데이터의 총 크기 계산
        int headerSize = sizeof(MsgType) + sizeof(uint16_t);
        int dataSize = playerCount * sizeof(PlayerSnapshot);
        int sendSize = headerSize + dataSize;

        RawPacketBuffer buffer;
        buffer.length = sendSize;

        // 2. 메모리 복사 (구조체 -> 버퍼)
        if (sendSize <= MAX_PACKET_SIZE)
        {
            std::memcpy(buffer.data, &snapshotPkt, sendSize);

            // 3. 큐에 넣기
            m_pOutputQueue->Push(buffer);
        }
        else
        {
            printf("[Error] Packet too big!\n");
        }
    }
}
// --- �̺�Ʈ �ڵ鷯 ���� ---
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
        if (!it->second->IsAlive())
        {
            return;
        }

        it->second->ApplyMovementFromClient(pkt, nowSec);
    }
}

void WorldState::OnFire(PlayerID pid, const C2S_FireAction& pkt, float nowSec)
{
	// ���� �� �÷��̾� ���� ������Ʈ
    auto it = m_PlayerStates.find(pid);
    if (it != m_PlayerStates.end())
    {
        it->second->ApplyFireFromClient(pkt);
    }

    // ���� ���� �÷��̾� ���� ������Ʈ
    if (pkt.hitPlayerID != -1)
    {
        // �ڱ� �ڽ��� ��� ��� ����
        if (pkt.hitPlayerID == pid) return;

        // ���� ��� ã��
        auto itVictim = m_PlayerStates.find(pkt.hitPlayerID);
        if (itVictim != m_PlayerStates.end())
        {
            // ���� ������� ������ ����
            itVictim->second->ApplyDamage(20, nowSec);
        }
    }

    // 3. [�ű� ����] �߻� ����� ��� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
    //    S2C_FireEvent ��Ŷ�� ����� ��� ť�� �ֽ��ϴ�.
    S2C_FireEvent fireEvt;
    fireEvt.type = MsgType::S2C_FIRE_EVENT;
    fireEvt.shooterID = pid;
    fireEvt.origin = pkt.fireOrigin;
    fireEvt.direction = pkt.fireDirection;
    fireEvt.hitPlayerID = pkt.hitPlayerID;

    // ��Ŷ ���ۿ� ���
    RawPacketBuffer buffer;
    buffer.length = sizeof(S2C_FireEvent);
    std::memcpy(buffer.data, &fireEvt, buffer.length);

    // ��� ť�� ������ NetworkIO �����尡 ��� Ŭ���̾�Ʈ���� �����մϴ�.
    if (m_pOutputQueue) {
        m_pOutputQueue->Push(buffer);
        printf("[WorldState] FireEvent Queued for broadcast (Shooter: %d)\n", pid);
    }
}

// �赵��
int WorldState::BuildSnapshotAll(float nowSec, S2C_SnapshotState& outPkt)
{
    outPkt.type = MsgType::S2C_SNAPSHOT_STATE;

	int count = 0;

    for (auto& pair : m_PlayerStates)
    {
        PlayerState* pState = pair.second;

        if (pState != nullptr)
        {
            // 1. PlayerState ������ ���� ����
            PlayerSnapshot snapshot = pState->ToSnapshot();

            // 2. ���� �ð� ����
            snapshot.serverTime = nowSec;

            // 3. ��Ŷ ���Ϳ� �߰�
            outPkt.snapshots[count] = snapshot;

            count++;
        }
    }

	return count;
}