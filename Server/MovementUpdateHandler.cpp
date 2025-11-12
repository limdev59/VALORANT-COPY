#include "MovementUpdateHandler.h"
#include "ClientSession.h"
#include "PacketDefs.h"
#include "DecodedPacket.h"
#include <cstdint>
#include <cstring>

bool MovementUpdateHandler::Parse(const uint8_t* data, size_t len, ClientSession& session, DecodedPacket& outPacket)
{
	if (len < sizeof(C2S_MovementUpdate))
		return false;

	const C2S_MovementUpdate* pkt = reinterpret_cast<const C2S_MovementUpdate*>(data);

	if (pkt->type != MsgType::C2S_MOVEMENT_UPDATE)
		return false;

	if (pkt->playerId != session.GetPlayerID())
		return false;

	outPacket.type = pkt->type;
	outPacket.movement = *pkt;

	return true;
} // 2025.11.12 지훈 -> 클라이언트의 움직임 업데이트 패킷을 파싱
