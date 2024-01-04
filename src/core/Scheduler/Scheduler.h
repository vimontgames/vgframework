#pragma once

#include "core/IScheduler.h"
#include "core/Singleton/Singleton.h"
#include "core/Scheduler/Mutex.h"

namespace px_sched
{
    class Scheduler;
}

namespace vg::core 
{
    class Job;

    class Scheduler final : public IScheduler, public core::Singleton<Scheduler>
    {
    public:
                                    Scheduler                   ();
                                    ~Scheduler                  ();

        void                        Start                       (Job * _job, JobSync * _sync) final override;
        JobSync                     Start                       (Job * _job) final override;
        void                        StartAfter                  (JobSync * _trigger, Job * _job, JobSync * _sync) final override;

        void                        Wait                        (JobSync * _sync) final override;

        void                        RegisterWorkerThreads       () final override;
        virtual void                RegisterCurrentThread       (const core::string & _name) final override;

        ThreadID                    GetCurrentThreadID          () const final override;
        const string                GetCurrentThreadName        () const final override;

        core::uint                  GetWorkerThreadCount        () const final override;

    private:
        px_sched::Scheduler *                               m_schd = nullptr;
        core::uint                                          m_threadCount = 0;
        core::unordered_map<ThreadID, const core::string>   m_registeredThreads;
        std::mutex                                          m_registerThreadMutex;
    };
}