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

    struct ThreadInfo
    {
        ThreadInfo(ThreadType _type, const core::string & _name) :
            m_type(_type),
            m_name(_name)
        {

        }
        const ThreadType m_type;
        const core::string m_name;
    };

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
        virtual void                RegisterCurrentThread       (const core::string & _name, ThreadType _threadType, core::uint _index, core::uint _count) final override;

        ThreadID                    GetCurrentThreadID          () const final override;
        ThreadType                  GetCurrentThreadType        () const final override;
        const string &              GetCurrentThreadName        () const final override;

        ThreadType                  GetThreadType               (ThreadID _threadID) const final override;
        const core::string &        GetThreadName               (ThreadID _threadID) const final override;

        core::uint                  GetWorkerThreadCount        () const final override;

        bool                        IsMainThread                () const final override;
        bool                        IsLoadingThread             () const final override;
        bool                        IsWorkerThread              () const final override;

    private:
        const ThreadInfo *          getThreadInfo               (ThreadID _threadID) const;
        const ThreadInfo *          getCurrentThreadInfo        () const;

    private:
        px_sched::Scheduler *                               m_schd = nullptr;
        core::uint                                          m_threadCount = 0;
        core::unordered_map<ThreadID, const ThreadInfo>     m_registeredThreads;
        mutable core::Mutex                                 m_registerThreadMutex = core::Mutex("RegisterThread");
    };
}