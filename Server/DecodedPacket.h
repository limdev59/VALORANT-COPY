#pragma once
#include "PacketDefs.h"

struct DecodedPacket
{
	MsgType type{};
	C2S_MovementUpdate movement;
};