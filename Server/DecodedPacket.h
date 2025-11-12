#pragma once
#include "PacketDefs.h"

struct DecodedPacket
{
	MsgType type{};
	C2S_MovementUpdate movement;
};	// 2025.11.12 지훈 -> 디코딩된 패킷을 담기 위한 구조체