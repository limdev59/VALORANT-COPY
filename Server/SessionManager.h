#pragma once
#include "ClientSession.h"
#include "common.h"
#include <map>
#include "QueueTypes.h"

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	void Initialize(PacketQueue* worldInQueue);
	void OnSessionLoggedIn(ClientSession* session, const sockaddr_in& tcpAddr, USHORT clientUdpPort);
	void OnSessionDisconnected(PlayerID pid);
	std::vector<ClientSession*> GetAllSessions();
	PlayerID FindPlayerIDByUdpAddress(const sockaddr_in& addr);
private:
	CRITICAL_SECTION m_Lock;
	std::map<PlayerID, ClientSession*> m_SessionsByPID;
	PacketQueue* m_pWorldInputQueue;
	PlayerID m_NextPlayerID;
};

