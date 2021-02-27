#pragma once

namespace vg::core
{
    class IScheduler
    {
    public:
        virtual ~IScheduler() = default;

        virtual void registerProfilerThreads() = 0;
    };
}