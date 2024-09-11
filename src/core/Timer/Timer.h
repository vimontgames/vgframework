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
        static double getCPUFrequency() { return s_freq; }
        
    private:
        static double s_freq;
    };
}