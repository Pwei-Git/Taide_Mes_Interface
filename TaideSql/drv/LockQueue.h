#pragma once

//////////////////////////////////////////////////////////////////////////
/// Include files
#include <deque>
#include "CSimpleLock.h"
#include "CLocks.h"

//////////////////////////////////////////////////////////////////////////
/// FIFO thread safe 
template <typename T>
class CLockQueue
{
public:
    CLockQueue(void)
    { 
        m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL); 
    }

    ~CLockQueue(void)
    {
        ::CloseHandle(m_hEvent);
    }
    
    // clear queue
    void clear(void) 
    {
        CLocks Lock(m_cs);
        m_deque.clear();
    }

    // return the count of elements
    int size(void) { return m_deque.size(); }

    // push back one element into queue
    void push_back(const T& data) 
    {
        CLocks Lock(m_cs);
        m_deque.push_back(data);
        wakeup();
    }

    // pop the front element of queue
    BOOL pop_front(T& data, unsigned long dwTimeOut = 0) 
    {
        BOOL bFound = FALSE;
        if (0 == dwTimeOut)
        {
            CLocks Lock(m_cs);
            bFound = get_front(data);
            check(); // wake up 
        }
        else
        {
            if (WAIT_OBJECT_0 == ::WaitForSingleObject(m_hEvent, dwTimeOut))
            {
                CLocks Lock(m_cs);
                bFound = get_front(data);
                check();
            }
        }

        return bFound;
    }

    // pop all of the elements out into a deque
    int  pop_all(std::deque<T>& dqe, unsigned long dwTimeOut = 0) 
    {
        int nCount = 0;
        if (0 == dwTimeOut)
        {
            CLocks Lock(m_cs);
            nCount = m_deque.size();
            if (nCount > 0)
            {
                dqe.assign(m_deque.begin(), m_deque.end());
                m_deque.clear();
            }
            ::ResetEvent(m_hEvent);
        }
        else
        {
            if (WAIT_OBJECT_0 == ::WaitForSingleObject(m_hEvent, dwTimeOut))
            {
                CLocks Lock(m_cs);
                nCount = m_deque.size();
                if (nCount > 0)
                {
                    dqe.assign(m_deque.begin(), m_deque.end());
                    m_deque.clear();
                }
                ResetEvent(m_hEvent);
            }
        }

        return nCount;
    }

    // swap a deque 
    void swap(std::deque<T>& dqe)
    {
        CLocks Lock(m_cs);
        m_deque.swap(dqe);
    }

    void wakeup(void) { ::SetEvent(m_hEvent); }

private:
    BOOL get_front(T& data)
    {
        if (m_deque.empty())
        {
            return FALSE;
        }
        else
        {
            data = m_deque.front();
            m_deque.pop_front();   
            return TRUE;
        }
    }

    void check(void) { m_deque.empty() ? ::ResetEvent(m_hEvent) : wakeup(); }

private:
    std::deque<T> m_deque;
    HANDLE  m_hEvent;
    CSimpleLock m_cs; 
};


