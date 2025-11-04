#pragma once
#include "PacketDefs.h"

class ClientSession
{
public:
    ClientSession()
        : playerId(nextId++)
    {
    }

	PlayerID GetPlayerID() const { return playerId; }   // 지훈   : 플레이어 ID 반환
    void SendUDP_SnapshotState(const std::vector<PlayerSnapshot>& snaps);

private:
    static inline PlayerID nextId = 0;
    PlayerID playerId;
};
