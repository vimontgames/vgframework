#pragma once

namespace vg::core
{
    using Ticks = i64;

    class Timer
    {
    public:
        static void init();

        static Ticks getTick();
        static double getEnlapsedTime(Ticks _start, Ticks _end);
        
    private:
        static double s_oneOverFrequency;
    };
}