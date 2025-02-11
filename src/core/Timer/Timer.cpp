#include "core/Precomp.h"
#include "Timer.h"

namespace vg::core
{
    double Timer::s_freq = 0.0f;

    //--------------------------------------------------------------------------------------
    void Timer::init()
    {
        s_freq = 1.0 / 1000.0;
    }

    //--------------------------------------------------------------------------------------
    Ticks Timer::getTick()
    {
		using namespace std::chrono;
        return duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }

    //--------------------------------------------------------------------------------------
    double Timer::getEnlapsedTime(Ticks _start, Ticks _end)
    {
        VG_ASSERT(s_freq > 0.0);
        return double(_end - _start) / s_freq;
    }
}