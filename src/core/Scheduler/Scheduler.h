#pragma once

#include "core/IScheduler.h"
#include "core/Singleton/Singleton.h"

namespace px_sched
{
    class Scheduler;
}

namespace vg::core 
{
    class Job;

    using JobSync = u64;

    class Scheduler : public IScheduler, public core::Singleton<Scheduler>
    {
    public:
        Scheduler();
        ~Scheduler();

        JobSync kickJob(Job * _job);

        void registerProfilerThreads() override;
        void test();

        static const string getCurrentThreadName();   

    private:
        void registerProfilerThreads(core::uint _count);

    private:
        px_sched::Scheduler * m_schd = nullptr;
        core::uint            m_threadCount = 0;
    };
}