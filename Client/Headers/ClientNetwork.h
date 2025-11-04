#pragma once
#pragma once

#include <string>
#include <vector>
#include <cstdint>

// --- 임시 구조체 선언 ---
// TODO: 나중에 실제 패킷 정의로 대체

/**
 * @brief @brief를 사용해 각 구조체와 클래스에 대한 간단한 설명을 추가하면
 * @brief 가독성이 좋아짐 읽어보길 바람
 */

struct MovementPacket {
    // C2S_MOVEMENT_UPDATE 필드
    uint32_t msgSeq;
    uint64_t playerId;

    // ... 기타 필드 (position, rotation, velocity, clientTime)
}; // 슝민

struct FirePacket {
    // C2S_FIRE_ACTION 필드
    uint32_t msgSeq;
    uint64_t playerId;

    // ... 기타 필드 (fireOrigin, fireDirection, clientTime)
};// 슝민

struct PlayerSnapshot {
    // S2C_SNAPSHOT_STATE 내 players 배열 요소
    uint64_t playerId;

    // ... 기타 상태 필드 (position, rotation, hp 등)
};// 슝민

/**
 * @brief 서버와의 모든 TCP/UDP 통신을 관리하는 클래스
 * @brief 제일 중요하니 구현 신경써서 함
 */
class ClientNetwork // 슝민
{
public:
    ClientNetwork();
    ~ClientNetwork();

    /**
     * @brief TCP/UDP 소켓을 생성하고 서버에 접속을 시도합니다.
     * @param ip 서버 IP 주소
     * @param tcpPort 서버 TCP 포트
     * @param udpPort 로컬에서 사용할 UDP 포트 (서버에 C2S_LOGIN_REQUEST로 전송)
     * @return 접속 성공 시 true, 실패 시 false
     */
    bool ConnectToServer(const std::string& ip, uint16_t tcpPort, uint16_t udpPort);// 슝민

    /**
     * @brief 서버로 이동 패킷(C2S_MOVEMENT_UPDATE)을 UDP로 전송합니다.
     * @param pkt Player 클래스에서 생성된 이동 패킷
     */
    void SendMovement(const MovementPacket& pkt);// 슝민

    /**
     * @brief 서버로 발사 패킷(C2S_FIRE_ACTION)을 UDP로 전송합니다.
     * @param pkt Player 클래스에서 생성된 발사 패킷
     */
    void SendFire(const FirePacket& pkt);// 슝민

    /**
     * @brief 비동기 수신된 패킷 큐를 확인하고 처리합니다. (주로 S2C_SNAPSHOT_STATE)
     * 이 함수는 매 프레임 호출되어야 합니다.
     */
    void PollIncomingPackets();// 슝민

    /**
     * @brief PollIncomingPackets()에 의해 수신된 가장 최신의 스냅샷을 반환
     * @return PlayerSnapshot의 const 참조 벡터
     */
    const std::vector<PlayerSnapshot>& GetLastSnapshots() const;// 슝민

private:


    // SOCKET m_tcpSocket;
    // SOCKET m_udpSocket;
    // SOCKADDR_IN m_serverUdpAddr;

    // 최신 스냅샷 저장용
    std::vector<PlayerSnapshot> m_lastSnapshots; // 슝민
};

// 임시 구조 정의임