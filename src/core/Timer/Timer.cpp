#include "core/Precomp.h"
#include "Timer.h"

#ifdef VG_WINDOWS
#define VG_USE_QUERYPERFORMANCECOUNTER 1
#endif

namespace vg::core
{
    double Timer::s_oneOverFrequency = 0.0f;

    //--------------------------------------------------------------------------------------
    void Timer::init()
    {
    #if VG_USE_QUERYPERFORMANCECOUNTER
        LARGE_INTEGER li;
        VG_VERIFY(QueryPerformanceFrequency(&li));
        s_oneOverFrequency = 1000.0 / double(li.QuadPart);
    #else
        s_oneOverFrequency = 1.0 / 1000.0;
    #endif
    }
    
    //--------------------------------------------------------------------------------------
    Ticks Timer::getTick()
    {
    #if VG_USE_QUERYPERFORMANCECOUNTER
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return li.QuadPart;
    #else
        using namespace std::chrono;
        return duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
    #endif
    }

    //--------------------------------------------------------------------------------------
    double Timer::getEnlapsedTime(Ticks _start, Ticks _end)
    {
        VG_ASSERT(s_oneOverFrequency != 0.0);
        return double(_end - _start) * s_oneOverFrequency;
    }
}