#pragma once
#include <cstdint>
#include <cstddef>

class ClientSession;
struct DecodedPacket;

class IPacketHandler
{
public:
	virtual ~IPacketHandler() = default;

	virtual bool Parse(const uint8_t* data, size_t len, ClientSession& session, DecodedPacket& outPacket) = 0;
};	// 2025.11.12 지훈 -> 패킷 파싱을 위한 인터페이스 클래스

