#include "pch.h"
#include "ClientNetwork.h"

#pragma comment(lib, "ws2_32.lib")


ClientNetwork::ClientNetwork() // 슝민
    : m_tcpSocket(INVALID_SOCKET)
    , m_udpSocket(INVALID_SOCKET)
    , m_myPlayerId(0)
{
    // 1. Winsock 초기화
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }
}

ClientNetwork::~ClientNetwork() // 슝민
{
    // 7. 소켓 정리
    if (m_tcpSocket != INVALID_SOCKET) {
        closesocket(m_tcpSocket);
    }
    if (m_udpSocket != INVALID_SOCKET) {
        closesocket(m_udpSocket);
    }

    // 8. Winsock 정리
    WSACleanup();
}

bool ClientNetwork::ConnectToServer(const std::string& ip, uint16_t tcpPort, uint16_t udpPort) // 슝민
{
    // 2. TCP 소켓 생성
    m_tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_tcpSocket == INVALID_SOCKET) {
        std::cerr << "TCP socket creation failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    // 3. 서버 주소 설정 (TCP)
    SOCKADDR_IN serverTcpAddr;
    serverTcpAddr.sin_family = AF_INET;
    serverTcpAddr.sin_port = htons(tcpPort);
    if (inet_pton(AF_INET, ip.c_str(), &serverTcpAddr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address: " << ip << std::endl;
        closesocket(m_tcpSocket);
        m_tcpSocket = INVALID_SOCKET;
        return false;
    }

    // 4. 서버에 TCP 접속
    // Server.cpp가 listen/accept를 구현하지 않았으므로,
    // 실제 실행 시 여기서 100% 실패
    if (connect(m_tcpSocket, (SOCKADDR*)&serverTcpAddr, sizeof(serverTcpAddr)) == SOCKET_ERROR) {
        std::cerr << "TCP connect failed (is server running and listening?): " << WSAGetLastError() << std::endl;
        closesocket(m_tcpSocket);
        m_tcpSocket = INVALID_SOCKET;
        return false;
    }

    std::cout << "TCP connected to server " << ip << ":" << tcpPort << std::endl;

    // 5. UDP 소켓 생성
    m_udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_udpSocket == INVALID_SOCKET) {
        std::cerr << "UDP socket creation failed: " << WSAGetLastError() << std::endl;
        closesocket(m_tcpSocket); 
        m_tcpSocket = INVALID_SOCKET;
        return false;
    }

    // 6. 로컬 UDP 주소 바인딩 (클라이언트가 패킷을 수신할 포트)
    SOCKADDR_IN localUdpAddr;
    localUdpAddr.sin_family = AF_INET;
    localUdpAddr.sin_port = htons(udpPort);
    localUdpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(m_udpSocket, (SOCKADDR*)&localUdpAddr, sizeof(localUdpAddr)) == SOCKET_ERROR) {
        std::cerr << "UDP bind failed: " << WSAGetLastError() << std::endl;
        closesocket(m_tcpSocket);
        closesocket(m_udpSocket);
        m_tcpSocket = INVALID_SOCKET;
        m_udpSocket = INVALID_SOCKET;
        return false;
    }

    std::cout << "UDP socket bound to local port " << udpPort << std::endl;

    // 7. 서버 UDP 주소 저장 (클라이언트가 패킷을 *송신*할 목적지 주소)
    //    서버의 UDP 수신 포트가 TCP 포트와 동일하다고 가정합니다
    //    만약 서버가 S2C_LOGIN_RESPONSE를 통해
    //    자신의 UDP 포트 번호를 알려준다면,
    //    이 코드는 S2C_LOGIN_RESPONSE 수신 이후에
    //    실행되어야 하며, `htons(tcpPort)` 대신
    //    응답받은 포트 번호를 사용
    m_serverUdpAddr.sin_family = AF_INET;
    m_serverUdpAddr.sin_port = htons(tcpPort);
    if (inet_pton(AF_INET, ip.c_str(), &m_serverUdpAddr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address for UDP: " << ip << std::endl;
        closesocket(m_tcpSocket);
        closesocket(m_udpSocket);
        m_tcpSocket = INVALID_SOCKET;
        m_udpSocket = INVALID_SOCKET;
        return false;
    }

    // 로그인
    // 8. C2S_LOGIN_REQUEST 패킷 생성 및 전송 (TCP)
    //    (PacketDefs.h에 C2S_LOGIN_REQUEST 구조체가 정의되어 있어야 함)
    std::cout << "TODO: Send C2S_LOGIN_REQUEST (TCP) with UDP port " << udpPort << std::endl;
    /*
    C2S_LOGIN_REQUEST loginPkt;
    loginPkt.header.packetType = PacketType::C2S_LOGIN_REQUEST;
    loginPkt.header.packetSize = sizeof(C2S_LOGIN_REQUEST);
    loginPkt.udpPort = udpPort; // 우리가 바인딩한 UDP 포트 알려주기

    if (send(m_tcpSocket, (char*)&loginPkt, sizeof(loginPkt), 0) == SOCKET_ERROR) {
        std::cerr << "send C2S_LOGIN_REQUEST failed: " << WSAGetLastError() << std::endl;
        // 소켓 정리 및 false 반환
        return false;
    }
    */

    // 9. S2C_LOGIN_RESPONSE 패킷 수신 (TCP) 및 m_myPlayerId 설정
    //    (PacketDefs.h에 S2C_LOGIN_RESPONSE 구조체가 정의되어 있어야 함)
    std::cout << "TODO: Receive S2C_LOGIN_RESPONSE (TCP) and set m_myPlayerId" << std::endl;
    /*
    S2C_LOGIN_RESPONSE loginResp;
    int bytesRecv = recv(m_tcpSocket, (char*)&loginResp, sizeof(loginResp), 0);
    
    if (bytesRecv <= 0 || loginResp.header.packetType != PacketType::S2C_LOGIN_RESPONSE) {
        std::cerr << "Failed to receive valid S2C_LOGIN_RESPONSE: " << WSAGetLastError() << std::endl;
        // 소켓 정리 및 false 반환
        return false;
    }
    m_myPlayerId = loginResp.playerId;
    std::cout << "Login successful. Player ID: " << m_myPlayerId << std::endl;
    */
    
    // TODO: TCP 소켓 논블로킹 전환 (PollIncomingPackets 위함)
    /*
    u_long nonBlockingMode = 1;
    if (ioctlsocket(m_tcpSocket, FIONBIO, &nonBlockingMode) == SOCKET_ERROR) {
       std::cerr << "ioctlsocket (non-blocking) failed: " << WSAGetLastError() << std::endl;
       // 오류 처리
       return false;
    }
    */

    // 로그인 로직이 구현되지 않았지만,
    // TCP 연결 및 UDP 바인딩까지 성공했다면 true 반환
    std::cout << "ConnectToServer finished (TODO: Login logic)." << std::endl;
    return true; 
} // 슝민

void ClientNetwork::SendMovement(const C2S_MovementUpdate& pkt) 
{
    if (m_udpSocket == INVALID_SOCKET) {
        std::cerr << "[이동] UDP 소켓이 유효하지 않습니다.\n";
        return;
    }

    // 구조체 메모리 그대로 송신
    const char* data = reinterpret_cast<const char*>(&pkt);
    int len = static_cast<int>(sizeof(pkt));

    int sent = sendto(
        m_udpSocket,
        data,
        len,
        0,
        reinterpret_cast<const sockaddr*>(&m_serverUdpAddr),
        sizeof(m_serverUdpAddr)
    );

    if (sent == SOCKET_ERROR) {
        std::cerr << "[이동] sendto 실패, WSA=" << WSAGetLastError() << "\n";
        return;
    }
}   // 2025.11.12 지훈 -> SendMovement 수정

void ClientNetwork::SendFire(const C2S_FireAction& pkt) // 슝민
{
    std::cout << "발사 패킷 전송 (아직 미구현)" << std::endl;
}

void ClientNetwork::PollIncomingPackets() // 슝민
{
    std::cout << "수신 패킷 처리 (미구현)" << std::endl;
}

const std::vector<PlayerSnapshot>& ClientNetwork::GetLastSnapshots() const // 슝민
{
    return m_lastSnapshots;
}