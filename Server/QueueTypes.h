#pragma once

#include "ThreadQueue.h" 
#include "WorldEvent.h"
#include <vector>

/**
 * @brief NetworkIO -> WorldState 입력 큐의 별명.
 */
typedef ThreadQueue<WorldEvent> PacketQueue;


/**
 * @brief WorldState -> NetworkIO 출력 큐의 별명.
 */
typedef ThreadQueue< std::vector<uint8_t> > ByteQueue;