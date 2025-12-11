#pragma once

#include "PacketDefs.h"   
#include "QueueTypes.h"   
#include "WorldEvent.h"  
#include "PlayerState.h"
#include <map>

/**
 * @class WorldState
 * @brief ��� PlayerState�� ����/�����ϰ� ť�� �̺�Ʈ�� ó��
 * �赵��
 */
class WorldState
{
public:
    WorldState();
    ~WorldState();

    /**
     * @brief ������ �� ��� ť ����
     */
    void Initialize(PacketQueue* inQ, ByteQueue* outQ);

    /**
     * @brief ���� ������ ���� ����. �Է� ť�� ó���ϰ�, �������� ��� ť�� ����
     */
    void Tick(float nowSec);
    /**
     * @brief ���� ������ ���۸� ��ȯ�ϴ� �Լ� - ����
     */
    // const std::deque<S2C_SnapshotState>& GetLastSnapshots() const { return m_SnapshotHistory; }

private:
    // --- �̺�Ʈ ó�� �Լ� ---
    void OnPlayerJoined(PlayerID pid);
    void OnPlayerLeft(PlayerID pid);
    void OnMovement(PlayerID pid, const C2S_MovementUpdate& pkt, float nowSec);
    void OnFire(PlayerID pid, const C2S_FireAction& pkt, float nowSec);

    /**
     * @brief ��� PlayerState�� ������ ����
     */
    int BuildSnapshotAll(float nowSec, S2C_SnapshotState& outPkt);

private:
    PacketQueue* m_pInputQueue;  // NetworkIO -> WorldState
    ByteQueue* m_pOutputQueue; // WorldState -> NetworkIO

    // ���ӿ� �����ϴ� ��� �÷��̾��� ����
    std::map<PlayerID, PlayerState*> m_PlayerStates;

    //// ������ �����丮 ���� (�ֱ� N���� ������ ����)
    //std::deque<S2C_SnapshotState> m_SnapshotHistory;

    //const size_t MAX_SNAPSHOT_HISTORY_SIZE = 60;
};