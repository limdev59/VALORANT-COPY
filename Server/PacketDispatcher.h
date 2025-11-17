#pragma once

#include "PacketDefs.h"       
#include "IPacketHandler.h"   
#include "QueueTypes.h"       
#include "ClientSession.h"    
#include <map>                

// UDP 패킷을 식별하고 파싱하여 WorldState 큐에 넣는 클래스 
// 김도윤
class PacketDispatcher
{
public:
    PacketDispatcher();
    ~PacketDispatcher();

    void Initialize(PacketQueue* worldInQueue);

    void DispatchUDP(const uint8_t* data, size_t len, ClientSession* session, float serverNow);

private:
    PacketQueue* m_pWorldInputQueue;

    std::map<MsgType, IPacketHandler*> m_Handlers;
};

