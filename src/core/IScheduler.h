#pragma once

namespace vg::core
{
    class Job;
    struct JobSync
    {
        u64 handle = 0; // JobHandle must be initialized
    };
    using ThreadID = std::thread::id;

    class IScheduler
    {
    public:
        virtual                         ~IScheduler             () = default;

        virtual void                    Start                   (Job * _job, JobSync * _sync) = 0;
        virtual JobSync                 Start                   (Job * _job) = 0; 
        virtual void                    StartAfter              (JobSync * _trigger, Job * _job, JobSync * _sync) = 0;
    
        virtual void                    Wait                    (JobSync * _sync) = 0;

        virtual void                    RegisterWorkerThreads   () = 0;
        virtual void                    RegisterCurrentThread   (const core::string & _name) = 0;

        virtual ThreadID                GetCurrentThreadID      () const = 0;
        virtual const core::string      GetCurrentThreadName    () const = 0;

        virtual core::uint              GetWorkerThreadCount    () const = 0;
    };
}