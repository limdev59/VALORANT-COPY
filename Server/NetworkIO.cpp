#include "NetworkIO.h"
#include <cstdio>
#include <vector>
#include "WorldEvent.h"
#include "PacketDefs.h"
#include "SessionManager.h"
#include "ThreadQueue.h"

NetworkIO::NetworkIO()
	: m_TCPListenSocket(INVALID_SOCKET)
	, m_UDPGameSocket(INVALID_SOCKET)
	, m_Isrunning(false)
	, m_pSessionManager(nullptr)
	, m_pWorldInputQueue(nullptr)
	, m_pNetworkOutputQueue(nullptr)

{
}

NetworkIO::~NetworkIO()
{
	m_Isrunning = false;

	if (m_TCPListenSocket != INVALID_SOCKET)
	{
		closesocket(m_TCPListenSocket);
		m_TCPListenSocket = INVALID_SOCKET;
	}

	if (m_UDPGameSocket != INVALID_SOCKET)
	{
		closesocket(m_UDPGameSocket);
		m_UDPGameSocket = INVALID_SOCKET;
	}
}

bool NetworkIO::Initialize(USHORT port, PacketQueue* worldln, ByteQueue* netOut, SessionManager* manager)
{
	int retval;

	m_pWorldInputQueue		= worldln;
	m_pNetworkOutputQueue	= netOut;
	m_pSessionManager		= manager;

	// TCP listen
	m_TCPListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_TCPListenSocket == INVALID_SOCKET) return false;

	u_long mode = 1;
	if (ioctlsocket(m_TCPListenSocket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		printf("[NetworkIO] TCP ioctlsocket failed.\n");
		return false;
	}

	sockaddr_in tcpAddr{};
	tcpAddr.sin_family		= AF_INET;
	tcpAddr.sin_port		= htons(port);
	tcpAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_TCPListenSocket, reinterpret_cast<sockaddr*>(&tcpAddr), sizeof(tcpAddr)) == SOCKET_ERROR)
		return false;

	if (listen(m_TCPListenSocket, SOMAXCONN) == SOCKET_ERROR)
		return false;

	if (m_TCPListenSocket == INVALID_SOCKET)
		return false;

	// UDP socket
	m_UDPGameSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_UDPGameSocket == INVALID_SOCKET) return false;

	// Non-Blocking 모드로 전환
	mode = 1; // 1: Non-Blocking, 0: Blocking
	if (ioctlsocket(m_UDPGameSocket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		printf("[NetworkIO] UDP ioctlsocket failed.\n");
		return false;
	}

	printf("[NetworkIO] UDP Socket created.\n");

	sockaddr_in udpAddr{};
	udpAddr.sin_family		= AF_INET;
	udpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udpAddr.sin_port		= htons(port);

	retval = bind(m_UDPGameSocket, reinterpret_cast<sockaddr*>(&udpAddr), sizeof(udpAddr));
	if (retval == SOCKET_ERROR)
		return false;
	printf("[NetworkIO] UDP Socket bound to port %d.\n", port);

	m_Isrunning = true;

	return true;
}

void NetworkIO::Run()
{
	// 단일 스레드 select 루프
	while (m_Isrunning)
	{
		fd_set readSet;
		FD_ZERO(&readSet);

		if (m_TCPListenSocket != INVALID_SOCKET)
			FD_SET(m_TCPListenSocket, &readSet);

		if (m_UDPGameSocket != INVALID_SOCKET)
			FD_SET(m_UDPGameSocket, &readSet);

		// 타임아웃 짧게 주고, 주기적으로 송신큐 브로드캐스트
		timeval timeout{};
		timeout.tv_sec = 0;
		timeout.tv_usec = 10 * 1000; // 10ms

		// windows select는 nfds 매개변수를 무시함
		int ret = select(0, &readSet, nullptr, nullptr, &timeout);
		if (ret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			printf("[NetworkIO] select() failed with error: %d\n", err);
			continue;
		}

		if (ret > 0)
		{
			if (m_TCPListenSocket != INVALID_SOCKET && FD_ISSET(m_TCPListenSocket, &readSet))
			{
				HandleTCPAccept();
			}

			if (m_UDPGameSocket != INVALID_SOCKET && FD_ISSET(m_UDPGameSocket, &readSet))
			{
				HandleUDPRead();
			}
		}

		// 송신 큐에 쌓인 데이터 브로드캐스트
		HandleBroadcasts();
	}

}

void NetworkIO::HandleTCPAccept()
{
	sockaddr_in clientAddr{};
	int clientAddrLen = sizeof(clientAddr);

	for (;;)
	{
		SOCKET clientSocket = accept(m_TCPListenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEINTR)
			{
				// 더 이상 대기중인 연결 없음
				printf("No more pending connections to accept.\n");
				break;
			}

		}
		// 2025.12.01 도윤 - 연결 성공 시 수행할 초기화 작업 추가 필요
		else {
			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

			printf("[NetworkIO] New TCP Connection from %s\n", clientIP);

			u_long blockMode = 0; // 0 = Blocking
			if (ioctlsocket(clientSocket, FIONBIO, &blockMode) == SOCKET_ERROR)
			{
				printf("ioctlsocket failed to set blocking mode\n");
				closesocket(clientSocket);
				continue;
			}

			// 새로운 ClientSession 생성 및 스레드 시작
			ClientSession* newSession = new ClientSession(clientSocket, m_pSessionManager, m_pWorldInputQueue);
			newSession->StartThread();
		}
	}
}

void NetworkIO::HandleUDPRead()
{
	char buffer[1500];
	sockaddr_in senderAddr{};
	int fromLen = sizeof(senderAddr);

	for (;;)
	{
		int n = recvfrom(m_UDPGameSocket, buffer, sizeof(buffer), 0,
			reinterpret_cast<sockaddr*>(&senderAddr), &fromLen);

		// 오류 처리
		if (n == SOCKET_ERROR)
		{
			int e = WSAGetLastError();
			if (e == WSAEWOULDBLOCK || e == WSAEINTR)
			{
				// 더 이상 읽을 데이터 없음
				break;
			}

			printf("[NetworkIO] UDP recvfrom failed code: %d\n", e);
			break;
		}

		if (n <= 0) break;

		// 성공 로직
		// 송신자 주소로부터 PlayerID 찾기
		PlayerID pid = static_cast<PlayerID>(-1);
		if (m_pSessionManager)
		{
			pid = m_pSessionManager->FindPlayerIDByUdpAddress(senderAddr);
		}

		if (pid == static_cast<PlayerID>(-1))
		{
			// 알 수 없는 송신자
			printf("[NetworkIO] Received UDP packet from unknown sender.\n");
			continue;
		}

		if (n < sizeof(MsgType))
		{
			// 패킷이 너무 작음
			printf("[NetworkIO] Received UDP packet too small.\n");
			continue;
		}

		// 패킷 타입에 따라 처리
		const MsgType* pType = reinterpret_cast<const MsgType*>(buffer);

		WorldEvent event{};
		event.playerID = pid;

		bool bValidPacket = false;

		switch (*pType)
		{
		case MsgType::C2S_MOVEMENT_UPDATE:
		{
			if (n < sizeof(C2S_MovementUpdate))
			{
				printf("[NetworkIO] Invalid MovementUpdate packet size.\n");
				break;
			}
			event.type = WorldEventType::E_Packet_Movement;
			std::memcpy(&event.movement, buffer, sizeof(C2S_MovementUpdate));
			bValidPacket = true;
			printf("[Server] Move Recv - PID:%d | Pos(%.2f, %.2f, %.2f)\n",
				pid,
				event.movement.position.x,
				event.movement.position.y,
				event.movement.position.z
			);
		}
		break;

		case MsgType::C2S_FIRE_ACTION:
		{
			if (n < sizeof(C2S_FireAction))
			{
				printf("[NetworkIO] Invalid Movement Packet Size..\n");
				break;
			}

			event.type = WorldEventType::E_Packet_Fire;
			std::memcpy(&event.fire, buffer, sizeof(C2S_FireAction));
			bValidPacket = true;
		}
		break;

		default:
			// 관심 없는 패킷 타입이면 무시
			break;
		}

		if (bValidPacket && m_pWorldInputQueue)
		{
			m_pWorldInputQueue->Push(event);
		}
	}
}

void NetworkIO::HandleBroadcasts()
{
	//if(!m_pNetworkOutputQueue || !m_pSessionManager|| m_UDPGameSocket == INVALID_SOCKET)
	//	return;

	//std::vector<uint8_t> bytes;

	//// 큐에 쌓인 데이터 모두 처리
	//while (m_pNetworkOutputQueue->TryPop(bytes))
	//{
	//	// 모든 세션에 브로드캐스트
	//	std::vector<sockaddr_in> clientAddrs = m_pSessionManager->GetAllClientUdpAddresses();
	//	for (const auto& addr : clientAddrs)
	//	{
	//		int n = sendto(m_UDPGameSocket,
	//			reinterpret_cast<const char*>(bytes.data()),
	//			static_cast<int>(bytes.size()),
	//			0,
	//			reinterpret_cast<const sockaddr*>(&addr),
	//			sizeof(addr));
	//		if (n == SOCKET_ERROR)
	//		{
	//			int err = WSAGetLastError();
	//			printf("sendto() failed with error: %d\n", err);
	//		}
	//	}
	//}

	
}

// 2025.11.12 지훈 -> NetworkIO.cpp 작성 완료, SessionManager와의 연동 필요
