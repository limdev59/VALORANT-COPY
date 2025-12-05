#pragma once
#include "PacketDefs.h"

struct DecodedPacket
{
	MsgType type{};
	C2S_MovementUpdate movement;
	C2S_FireAction fire;		
}; // 2025.11.12 수정 -> 리팩토링 패키지 구조 변경
