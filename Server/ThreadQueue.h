#pragma once

#include "common.h"
#include <queue>

/**
 * @class ThreadQueue
 * @brief CRITICAL_SECTION으로 보호하는 큐
 * 하나의 스레드만 큐에 접근 가능
 * @tparam T 큐에 저장할 요소의 타입 
 * 
 * 김도윤
 */
template<typename T>
class ThreadQueue
{
public:
    /**
     * @brief 생성자: 임계 영역 초기화
     */
    ThreadQueue()
    {
        InitializeCriticalSection(&m_CS);
    }

    /**
     * @brief 소멸자: 임계 영역 해제
     */
    ~ThreadQueue()
    {
        DeleteCriticalSection(&m_CS);
    }

    // 클래스 복사를 막는 안전장치
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    /**
     * @brief 큐의 뒤에 아이템 추가
     */
    void Push(T item)
    {
        EnterCriticalSection(&m_CS);
        m_Queue.push(item);
        LeaveCriticalSection(&m_CS);
    }

    /**
     * @brief 큐 채워져 있으면, 아이템을 꺼내고 true /
     * 비어있다면, false
     */
    bool TryPop(T& outItem)
    {
        EnterCriticalSection(&m_CS);
        if (m_Queue.empty())
        {
            LeaveCriticalSection(&m_CS);
            return false;
        }
        outItem = m_Queue.front();  // 값 복사
        m_Queue.pop();
        LeaveCriticalSection(&m_CS);
        return true;
    }

private:

    std::queue<T>    m_Queue; // 실제 데이터를 담는 큐
    CRITICAL_SECTION m_CS;    // 큐 보호를 위한 임계 영역
};

