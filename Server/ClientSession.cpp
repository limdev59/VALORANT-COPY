#include "ClientSession.h"
#include "SessionManager.h"
#include "PacketDefs.h"
#include "common.h"

ClientSession::ClientSession(SOCKET tcpSocket, SessionManager* manager, PacketQueue* worldlnQueue)
	: m_TCPSocket(tcpSocket)
	, m_hThread(nullptr)
	, m_PlayerID(static_cast<PlayerID> (-1))
	, m_pSessionManager(manager)
	, m_pWorldlnputQueue(worldlnQueue)
{
	std::memset(&m_UDPEndPoint, 0, sizeof(m_UDPEndPoint));
}

ClientSession::~ClientSession()
{
	// 스레드 종료 대기 및 핸들 정리
	if (m_hThread != nullptr)
	{
		// 소켓 에러 -> HandleDisconnect() 호출
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = nullptr;
	}

	// 소켓이 살아있다면 대기
	if (m_TCPSocket != INVALID_SOCKET)
	{
		closesocket(m_TCPSocket);
		m_TCPSocket = INVALID_SOCKET;
	}
}

// 스레드 시작
void ClientSession::StartThread()
{
	// this 포인터 인자로 넘겨 ThreadWrapper에서 다시 멤버 함수 호출
	m_hThread = CreateThread(nullptr, 0, ThreadWrapper, this, 0, nullptr);

	if (m_hThread == nullptr)
	{
		printf("[ClientSession] CreateThread Faill\n");
		HandleDisconnect();
	}
}

void ClientSession::SetLoginInfo(PlayerID id, const sockaddr_in& udpAddr)
{
	m_PlayerID	  = id;
	m_UDPEndPoint = udpAddr;
}

// 정적 스레드 핸들러
DWORD WINAPI ClientSession::ThreadWrapper(LPVOID lpParam)
{
	ClientSession* self = reinterpret_cast<ClientSession*>(lpParam);
	if (self)
	{
		self->ReceiveLoop();
	}

	return 0;
}

// TCP 수신 루프
void ClientSession::ReceiveLoop()
{
	printf("[ClientSession] ReceiveLoop 시작\n");

	// 로그인 패킷 받기
	C2S_LoginRequest loginPkt{};
	char* writePtr	= reinterpret_cast<char*>(&loginPkt);
	int	  total		= 0;
	int	  target	= static_cast<int>(sizeof(loginPkt));

	while (total < target)
	{
		int ret = recv(m_TCPSocket, writePtr + total, target - total, 0);
		if (ret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			printf("[ClientSession] recv(login) Error: %d\n", err);
			HandleDisconnect();
			return;
		}

		if (ret == 0)
		{
			// 원격에서 정상 종료
			printf("[ClientSession] 로그인 전에 종료\n");
			return;
		}

		total += ret;
	}

	// 로그인 패킷 확인
	printf("[ClientSession] 로그인 요청 받음: playerName='%s', clientUdpPort=%d\n",
		loginPkt.playerName.c_str(), loginPkt.clientUdpPort);

	// 로그인
	HandleLogin(loginPkt);

	// TODO: 로그인 이후 확인 패킷 보내기

	// 이후에 올 게 없다고 가정하고 끊김만 감지하는 루프 (필요시 패킷 처리 추가)
	char buf[512];
	while (true)
	{
		int ret = recv(m_TCPSocket, buf, sizeof(buf), 0);
		if (ret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			printf("[ClientSession] recv(후속) Error: %d\n", err);
			HandleDisconnect();
			return;
		}

		if (ret == 0)
		{
			// 정상 종료
			printf("[ClientSession] 클라이언트 정상 종료\n");
			HandleDisconnect();
			return;
		}
	}
}

// 로그인 패킷 처리
void ClientSession::HandleLogin(const C2S_LoginRequest& pkt)
{
	// TCP 원격 주소 얻기
	sockaddr_in tcpAddr{};
	int addrLen = sizeof(tcpAddr);
	std::memset(&tcpAddr, 0, sizeof(tcpAddr));

	if (getpeername(m_TCPSocket, reinterpret_cast<sockaddr*>(&tcpAddr), &addrLen) == SOCKET_ERROR)
	{
		printf("[ClientSession] getpeername() Fail, err = %d\n", WSAGetLastError());
		HandleDisconnect();
		return;
	}

	// SessionManager에게 로그인 완료 보고 (PlayerID 배정 + UDP 엔드포인트 계산)
	if (m_pSessionManager)
	{
		m_pSessionManager->OnSessionLoggedIn(this, tcpAddr, pkt.clientUdpPort);
	}
}

// 연결 종료 처리
void ClientSession::HandleDisconnect()
{
	// PlayerID가 이미 배정된 상태라면 SessionManger에 전달
	if (m_pSessionManager && m_PlayerID != static_cast<PlayerID>(-1))
	{
		m_pSessionManager->OnSessionDisconnected(m_PlayerID);
		m_PlayerID = static_cast<PlayerID>(-1);
	}

	if (m_TCPSocket != INVALID_SOCKET)
	{
		closesocket(m_TCPSocket);
		m_TCPSocket = INVALID_SOCKET;
	}
}

