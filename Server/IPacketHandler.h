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
};

