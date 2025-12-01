#include "Common.h"
#include "NetworkIO.h"
#include "SessionManager.h"
#include "WorldState.h"
#include "PacketDispatcher.h"

#include "QueueTypes.h"
#include <chrono>

// 서버 설정
const USHORT SERVER_PORT = 7777;
const float TARGET_FPS = 60.0f;
const float FRAME_DELAY_MS = 1000.0f / TARGET_FPS;

// 스레드 함수
DWORD WINAPI NetworkThreadFunc(LPVOID lpParam)
{
	NetworkIO* pNetworkIO = reinterpret_cast<NetworkIO*>(lpParam);
	if (pNetworkIO) {
		pNetworkIO->Run();
	}
	return 0;
}

int main(int argc, char* argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("[Error] WSAStartup Failed.\n");
		return 1;
	}
	printf("[System] Winsock Initialized.\n");

	// 2. 통신 큐 생성
	PacketQueue worldInputQueue;
	ByteQueue networkOutputQueue;

	// 3. 주요 매니저 객체 생성
	SessionManager sessionManager;
	WorldState worldState;
	NetworkIO networkIO;
	PacketDispatcher packetDispatcher;

	// 4. 초기화 및 연결
	sessionManager.Initialize(&worldInputQueue);
	printf("[System] SessionManager Initialized.\n");

	packetDispatcher.Initialize(&worldInputQueue);
	printf("[System] PacketDispatcher Initialized.\n");

	worldState.Initialize(&worldInputQueue, &networkOutputQueue);
	printf("[System] WorldState Initialized.\n");

	if (!networkIO.Initialize(SERVER_PORT, &worldInputQueue, &networkOutputQueue, &sessionManager))
	{
		printf("[Error] NetworkIO Initialize Failed. Port: %d\n", SERVER_PORT);
		return 1;
	}
	printf("[System] NetworkIO Initialized on Port %d.\n", SERVER_PORT);

    // 5. 네트워크 스레드 생성 (Win32 CreateThread 사용)
    HANDLE hNetworkThread = CreateThread(
        NULL,                   // 보안 속성
        0,                      // 스택 크기
        NetworkThreadFunc,      // 스레드 함수
        &networkIO,             // 파라미터
        0,                      // 생성 플래그
        NULL                    // 스레드 ID
    );

    if (hNetworkThread == NULL)
    {
        printf("[Error] CreateThread Failed. Error: %d\n", GetLastError());
        return 1;
    }
    printf("[System] Network Thread Started (Handle: %p).\n", hNetworkThread);

    // 6. 메인 스레드: 게임 로직 루프
    printf("[System] Starting Game Loop (Main Thread)...\n");

    // 시간 측정용 chrono 설정
    using namespace std::chrono;

    // 서버 시작 시간
    auto startTime = high_resolution_clock::now();

    while (true)
    {
        // 현재 프레임 시작 시간
        auto loopStart = high_resolution_clock::now();

        // 서버 시작 후 총 경과 시간 (초 단위) 계산 -> WorldState::Tick에 전달
        duration<float> elapsedSinceStart = loopStart - startTime;
        float totalTimeSec = elapsedSinceStart.count();

        // 게임 로직 업데이트
        worldState.Tick(totalTimeSec);

        // 프레임 제한
        auto loopEnd = high_resolution_clock::now();
        duration<float, std::milli> workTime = loopEnd - loopStart;

        // 목표 딜레이에서 실제 작업 시간을 뺀 만큼 대기
        float waitMs = FRAME_DELAY_MS - workTime.count();

        if (waitMs > 0)
        {
            Sleep(static_cast<DWORD>(waitMs));
        }
    }

    // 7. 종료 처리 (메인 루프가 무한이라 도달하진 않음)
    if (hNetworkThread != NULL)
    {
        WaitForSingleObject(hNetworkThread, INFINITE);
        CloseHandle(hNetworkThread);
    }

    WSACleanup();
    return 0;
}