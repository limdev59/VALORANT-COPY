#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include "QueueTypes.h"

class SessionManager;

class NetworkIO
{
public:
	NetworkIO();
	~NetworkIO();

	bool Initialize(USHORT port, PacketQueue* worldln, ByteQueue* netOut, SessionManager* manager);
	void Run();

private:
	void HandleTCPAccept();
	void HandleUDPRead();
	void HandleBroadcasts();

private:
	SOCKET m_TCPListenSocket;
	SOCKET m_UDPGameSocket;
	bool   m_Isrunning;

	SessionManager* m_pSessionManager;
	PacketQueue*	m_pWorldInputQueue;
	ByteQueue*		m_pNetworkOutputQueue;
	
};

