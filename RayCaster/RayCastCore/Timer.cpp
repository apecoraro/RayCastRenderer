#include "Timer.h"

#include <sys/types.h>
#include <fcntl.h>
#include <windows.h>
#include <winbase.h>

using namespace cs500;

Timer* Timer::instance()
{
    static Timer s_timer;
    return &s_timer;
}

Timer::Timer()
{
    LARGE_INTEGER frequency;
    if(QueryPerformanceFrequency(&frequency))
    {
        m_secsPerTick = 1.0/(double)frequency.QuadPart;
    }
    else
    {
        throw "Error ocurred trying to use QueryPerformanceFrequency(), Timer construction failed";
    }
}

Timer_t Timer::tick() const
{
    LARGE_INTEGER qpc;
    if (QueryPerformanceCounter(&qpc))
    {
        return qpc.QuadPart;
    }
    else
    {
        throw "Error ocurred trying to use QueryPerformanceCounter(), tick() failed";
    }
}