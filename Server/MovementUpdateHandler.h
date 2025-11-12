#pragma once
#include "IPacketHandler.h"

class MovementUpdateHandler : public IPacketHandler
{
	bool Parse(const uint8_t* data, size_t len, ClientSession& session, DecodedPacket& outPacket) override;

};

