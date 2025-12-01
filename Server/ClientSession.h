#pragma once
#include "PacketDefs.h"
#include "common.h"
#include "QueueTypes.h"

class SessionManager;

class ClientSession
{
public:
	ClientSession(SOCKET tcpSocket, SessionManager* manager, PacketQueue* worldlnQueue);
	~ClientSession();

	void StartThread();
	void SetLoginInfo(PlayerID id, const sockaddr_in& udpAddr);

	PlayerID GetPlayerID() const { return m_PlayerID; }   // 지훈   : 플레이어 ID 반환 -> 2025.11.17 변수명 교체
	sockaddr_in GetUdpAddress() const { return m_UDPEndPoint; }

private:
	static DWORD WINAPI ThreadWrapper(LPVOID lpParam);
	void ReceiveLoop();
	void HandleLogin(const C2S_LoginRequest& pkt);
	void HandleDisconnect();

private:
	SOCKET			m_TCPSocket;
	HANDLE			m_hThread;
	PlayerID		m_PlayerID;
	sockaddr_in		m_UDPEndPoint;
	SessionManager* m_pSessionManager;
	PacketQueue*	m_pWorldlnputQueue;
};