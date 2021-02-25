#pragma once

namespace vg::core
{
    class Timer
    {
    public:
        using Tick = i64;

        static void init();

        static Tick getTick();
        static double getEnlapsedTime(Tick _start, Tick _end);
        
    private:
        static double s_freq;
    };
}