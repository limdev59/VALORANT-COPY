#pragma once
#include "IPacketHandler.h"

class FireActionHandler : public IPacketHandler
{
public:
    // 인터페이스 구현
    bool Parse(const uint8_t* data, size_t len, ClientSession& session, DecodedPacket& outPacket) override;
};