#include "FireActionHandler.h"
#include "ClientSession.h"
#include "PacketDefs.h"
#include "DecodedPacket.h"
#include <cstring>

bool FireActionHandler::Parse(const uint8_t* data, size_t len, ClientSession& session, DecodedPacket& outPacket)
{
    // 1. 패킷 길이 검사
    if (len < sizeof(C2S_FireAction))
        return false;

    // 2. 패킷 포인터 캐스팅
    const C2S_FireAction* pkt = reinterpret_cast<const C2S_FireAction*>(data);

    // 3. 패킷 타입 검증
    if (pkt->type != MsgType::C2S_FIRE_ACTION)
        return false;

    // 4. 플레이어 ID 검증 (본인이 보낸 패킷인지)
    if (pkt->playerId != session.GetPlayerID())
        return false;

    // 5. DecodedPacket에 데이터 채우기
    outPacket.type = pkt->type;
    outPacket.fire = *pkt; // 구조체 복사

    return true;
}