#pragma once

namespace vg::core
{
    class Job;
    struct JobSync
    {
        u64 handle = 0; // JobHandle must be initialized
    };
    using ThreadID = std::thread::id;

    vg_enum_class(vg::core, ThreadType, i8,
        Unknown     = -128,
        Loading     = -2,
        Main        = -1,
        Worker      = 0
    );

    class IScheduler
    {
    public:
        virtual                         ~IScheduler             () = default;

        virtual void                    Start                   (Job * _job, JobSync * _sync) = 0;
        virtual JobSync                 Start                   (Job * _job) = 0; 
        virtual void                    StartAfter              (JobSync * _trigger, Job * _job, JobSync * _sync) = 0;
    
        virtual void                    Wait                    (JobSync * _sync) = 0;

        virtual void                    RegisterWorkerThreads   () = 0;
        virtual void                    RegisterCurrentThread   (const core::string & _name, ThreadType _threadType, core::uint _index, core::uint _count) = 0;

        virtual ThreadID                GetCurrentThreadID      () const = 0;
        virtual ThreadType              GetCurrentThreadType    () const = 0;
        virtual const core::string &    GetCurrentThreadName    () const = 0;

        virtual ThreadType              GetThreadType           (ThreadID _threadID) const = 0;
        virtual const core::string &    GetThreadName           (ThreadID _threadID) const = 0;

        virtual core::uint              GetWorkerThreadCount    () const = 0;

        virtual bool                    IsMainThread            () const = 0;
        virtual bool                    IsLoadingThread         () const = 0;
        virtual bool                    IsWorkerThread          () const = 0;
    };
}