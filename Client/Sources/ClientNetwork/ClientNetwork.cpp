#include "pch.h"
#include "ClientNetwork.h"
#include "PacketDefs.h"

// ¿œ¥‹ »§Ω√ ∏Ù∂Û ¡÷ºÆ¿∏∑Œ √ﬂ∞°
// #include <WinSock2.h>
// #include <WS2tcpip.h>
// #pragma comment(lib, "ws2_32.lib")


ClientNetwork::ClientNetwork() // ΩππŒ
{

}

ClientNetwork::~ClientNetwork() // ΩππŒ
{

}

bool ClientNetwork::ConnectToServer(const std::string& ip, uint16_t tcpPort, uint16_t udpPort) // ΩππŒ
{
    std::cout << "º≠πˆø° ¡¢º” Ω√µµ (æ∆¡˜ πÃ±∏«ˆ): " << ip << ":" << tcpPort << std::endl;
    return false; 
}

void ClientNetwork::SendMovement(const C2S_MovementUpdate& pkt) // ΩππŒ
{
    std::cout << "¿Ãµø ∆–≈∂ ¿¸º€ (æ∆¡˜ πÃ±∏«ˆ)" << std::endl;
}

void ClientNetwork::SendFire(const C2S_FireAction& pkt) // ΩππŒ
{
    std::cout << "πﬂªÁ ∆–≈∂ ¿¸º€ (æ∆¡˜ πÃ±∏«ˆ)" << std::endl;
}

void ClientNetwork::PollIncomingPackets() // ΩππŒ
{
    std::cout << "ºˆΩ≈ ∆–≈∂ √≥∏Æ (πÃ±∏«ˆ)" << std::endl;
}

const std::vector<PlayerSnapshot>& ClientNetwork::GetLastSnapshots() const // ΩππŒ
{
    return m_lastSnapshots;
}