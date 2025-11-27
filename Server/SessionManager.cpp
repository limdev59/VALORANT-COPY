#include "SessionManager.h"
#include "common.h"
#include "PacketDefs.h"
#include "ClientSession.h"
#include "WorldEvent.h"

SessionManager::SessionManager()
	: m_pWorldInputQueue(nullptr)
	, m_NextPlayerID(0)
{
	InitializeCriticalSection(&m_Lock);
}

SessionManager::~SessionManager()
{
	EnterCriticalSection(&m_Lock);

	for (auto& kv : m_SessionsByPID)
	{
		ClientSession* session = kv.second;
		delete session;
	}
	m_SessionsByPID.clear();

	LeaveCriticalSection(&m_Lock);

	DeleteCriticalSection(&m_Lock);
}

void SessionManager::Initialize(PacketQueue* worldInQueue)
{
	m_pWorldInputQueue = worldInQueue;
	m_NextPlayerID = 0;
}

void SessionManager::OnSessionLoggedIn(ClientSession* session, const sockaddr_in& tcpAddr, USHORT clientUdpPort)
{
	if (!session) return;

	// 접속 순서대로 PlayerID 부여
	EnterCriticalSection(&m_Lock);
	PlayerID newPid = m_NextPlayerID++;
	LeaveCriticalSection(&m_Lock);

	// 클라이언트 UDP 엔드포인트
	// IP는 TCP 원격 IP, 포트는 클라이언트가 보낸 clientUdpPort 사용
	sockaddr_in udpAddr{};
	udpAddr.sin_family	= AF_INET;
	udpAddr.sin_addr	= tcpAddr.sin_addr;
	udpAddr.sin_port	= htons(clientUdpPort);

	// 세션에 로그인 정보 세팅
	session->SetLoginInfo(newPid, udpAddr);

	// 세션 맵에 등록
	EnterCriticalSection(&m_Lock);
	m_SessionsByPID[newPid] = session;
	LeaveCriticalSection(&m_Lock);

	// 월드 스레드에 플레이어 접속 이벤트 보내기
	if (m_pWorldInputQueue)
	{
		WorldEvent ev{};
		ev.type			= E_PlayerJoined;
		ev.playerID		= newPid;

		m_pWorldInputQueue->Push(ev);
	}
}

void SessionManager::OnSessionDisconnected(PlayerID pid)
{
	ClientSession* session = nullptr;

	// 맵에서 제거
	EnterCriticalSection(&m_Lock);

	auto it = m_SessionsByPID.find(pid);
	if (it != m_SessionsByPID.end())
	{
		session = it->second;
		m_SessionsByPID.erase(it);
	}

	LeaveCriticalSection(&m_Lock);

	// 세션 삭제 및 월드에 이벤트 전달
	if (session)
	{
		delete session;

		if (m_pWorldInputQueue)
		{
			WorldEvent ev{};
			ev.type			= E_PlayerLeft;
			ev.playerID		= pid;

			m_pWorldInputQueue->Push(ev);
		}
	}
}

std::vector<ClientSession*> SessionManager::GetAllSessions()
{
	std::vector<ClientSession*> result;

	EnterCriticalSection(&m_Lock);

	result.reserve(m_SessionsByPID.size());
	for (auto& kv : m_SessionsByPID)
	{
		result.push_back(kv.second);
	}

	LeaveCriticalSection(&m_Lock);

	return result;
}

PlayerID SessionManager::FindPlayerIDByUdpAddress(const sockaddr_in& addr)
{
	PlayerID found = static_cast<PlayerID>(-1);

	EnterCriticalSection(&m_Lock);

	for (auto kv : m_SessionsByPID)
	{
		ClientSession* session = kv.second;
		if (!session) continue;

		sockaddr_in cliAddr = session->GetUdpAddress();

		if (cliAddr.sin_addr.s_addr == addr.sin_addr.s_addr && 
			cliAddr.sin_port		== addr.sin_port)
		{
			found = kv.first;
			break;
		}
	}
	LeaveCriticalSection(&m_Lock);
	return found;
}
 