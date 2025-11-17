#include "PacketDispatcher.h"
#include "WorldEvent.h"           
#include "DecodedPacket.h"        

#include "MovementUpdateHandler.h" 
// #include "FireActionHandler.h"  // [발사] 핸들러

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

    // 발사 패킷 핸들러 (Fire)
    // m_Handlers[MsgType::C2S_FIRE_ACTION] = new FireActionHandler();

    printf("[PacketDispatcher] Initialized. Handlers registered: %zu\n", m_Handlers.size());
}

void PacketDispatcher::DispatchUDP(const uint8_t* data, size_t len, ClientSession* session, float serverNow)
{
    if (len < sizeof(MsgType))
    {
        return;
    }

    // MsgType 읽어 패킷인지 확인
    MsgType type = *reinterpret_cast<const MsgType*>(data);

    // 해당 타입 핸들러 등록 확인
    auto it = m_Handlers.find(type);
    if (it == m_Handlers.end())
    {
        printf("[PacketDispatcher] Unknown or Unhandled UDP packet type: %d\n", (int)type);
        return;
    }

    IPacketHandler* handler = it->second;
    DecodedPacket decodedPkt; // 결과를 받을 구조체

    // 파싱
    bool success = handler->Parse(data, len, *session, decodedPkt);

    if (success)
    {
        // 파싱 성공 시, 이벤트 생성
        WorldEvent event;
        event.playerID = session->GetPlayerID(); // 누가 보냈는지 기록

        // 패킷 타입에 따라 이벤트 종류 설정 및 데이터 복사
        if (type == MsgType::C2S_MOVEMENT_UPDATE)
        {
            event.type = E_Packet_Movement;
            event.movement = decodedPkt.movement;
        }
        //else if (type == MsgType::C2S_FIRE_ACTION)
        //{
        //    event.type = E_Packet_Fire;
        //    event.fire = decodedPkt.fire;
        //}

        // 큐에 이벤트 푸시
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