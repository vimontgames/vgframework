#include "core/Precomp.h"
#include "Timer.h"

namespace vg::core
{
    double Timer::s_freq = 0.0f;

    //--------------------------------------------------------------------------------------
    void Timer::init()
    {
        LARGE_INTEGER li;
        VG_VERIFY(QueryPerformanceFrequency(&li));
        s_freq = double(li.QuadPart) / 1000.0;
    }

    //--------------------------------------------------------------------------------------
    Timer::Tick Timer::getTick()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        return li.QuadPart;
    }

    //--------------------------------------------------------------------------------------
    double Timer::getEnlapsedTime(Timer::Tick _start, Timer::Tick _end)
    {
        return double(_end - _start) / s_freq;
    }
}