#include "PacketDispatcher.h"
#include "WorldEvent.h"
#include "DecodedPacket.h"

#include "MovementUpdateHandler.h"
#include "FireActionHandler.h"

PacketDispatcher::PacketDispatcher()
    : m_pWorldInputQueue(nullptr)
{
}

PacketDispatcher::~PacketDispatcher()
{
    for (auto& pair : m_Handlers)
    {
        delete pair.second;
    }
    m_Handlers.clear();
}

void PacketDispatcher::Initialize(PacketQueue* worldInQueue)
{
    m_pWorldInputQueue = worldInQueue;

    // 이동 패킷 핸들러 (Movement)
    m_Handlers[MsgType::C2S_MOVEMENT_UPDATE] = new MovementUpdateHandler();

    // 사격 패킷 핸들러 (Fire)
    m_Handlers[MsgType::C2S_FIRE_ACTION] = new FireActionHandler();

    printf("[PacketDispatcher] Initialized. Handlers registered: %zu\n", m_Handlers.size());
}

void PacketDispatcher::DispatchUDP(const uint8_t* data, size_t len, ClientSession* session, float serverNow)
{
    if (len < sizeof(MsgType))
    {
        return;
    }

    // MsgType 으로 패킷 타입 확인
    MsgType type = *reinterpret_cast<const MsgType*>(data);

    // 등록된 타입에 대한 핸들러 존재 여부 확인
    auto it = m_Handlers.find(type);
    if (it == m_Handlers.end())
    {
        printf("[PacketDispatcher] Unknown or Unhandled UDP packet type: %d\n", (int)type);
        return;
    }

    IPacketHandler* handler = it->second;
    DecodedPacket decodedPkt; // 출력용

    // 파싱
    bool success = handler->Parse(data, len, *session, decodedPkt);

    if (success)
    {
        // 파싱 완료, 이벤트 생성
        WorldEvent event;
        event.playerID = session->GetPlayerID(); // 기본값

        // 패킷 타입별로 이벤트 구성
        if (type == MsgType::C2S_MOVEMENT_UPDATE)
        {
            event.type = E_Packet_Movement;
            event.movement = decodedPkt.movement;
        }
        else if (type == MsgType::C2S_FIRE_ACTION)
        {
            event.type = E_Packet_Fire;
            event.fire = decodedPkt.fire;
        }

        // 큐에 이벤트 밀어넣기
        if (m_pWorldInputQueue)
        {
            m_pWorldInputQueue->Push(event);
        }
    }
    else
    {
        printf("[PacketDispatcher] Failed to parse packet type: %d\n", (int)type);
    }
}
