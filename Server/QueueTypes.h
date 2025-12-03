#pragma once

#include "ThreadQueue.h" 
#include "WorldEvent.h"
#include <vector>

const int MAX_PACKET_SIZE = 2048;

struct RawPacketBuffer
{
    int length = 0;             
    uint8_t data[MAX_PACKET_SIZE]; 
};

/**
 * @brief NetworkIO -> WorldState 입력 큐의 별명.
 */
typedef ThreadQueue<WorldEvent> PacketQueue;


/**
 * @brief WorldState -> NetworkIO 출력 큐의 별명.
 */
typedef ThreadQueue<RawPacketBuffer> ByteQueue;