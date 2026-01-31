#include "core/Precomp.h"
#include "Scheduler.h"
#include "Job.h"
#include "core/IProfiler.h"
#include "core/string/string.h"

// Job definition must be done before including px_sched.h 
#define PX_SCHED_CUSTOM_JOB_DEFINITION
namespace px_sched 
{
    struct Job 
    {
        vg::core::Job * m_job = nullptr;
        void operator()() { m_job->RegisterAndRun(); }
    };
} // px namespace

#define PX_SCHED_IMPLEMENTATION 1
#pragma push_macro("new")
#undef new

#if VG_FINAL
#define PX_SCHED_DOES_CHECKS 0
#else
#define PX_SCHED_DOES_CHECKS 1
#define PX_SCHED_CHECK_FN(cond, ...) VG_ASSERT(cond, __VA_ARGS__)
//#define PX_SCHED_TRACE_FN(...) VG_PROFILE_CPU(__VA_ARGS__)
#endif

#include "px/px_sched.h"
#pragma pop_macro("new")


namespace vg::core
{
    //--------------------------------------------------------------------------------------
    ThreadID Scheduler::GetCurrentThreadID() const
    {
        return std::this_thread::get_id();
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::RegisterCurrentThread(const core::string & _name, ThreadType _threadType, core::uint _index, core::uint _count)
    {
        core::lock_guard lock(m_registerThreadMutex);

        // Current thread id
        ThreadID threadId = GetCurrentThreadID();

        // Check if already registered
        if (m_registeredThreads.end() != m_registeredThreads.find(threadId))
            return;  

        if (_count == 1)
            VG_DEBUGPRINT("[Profiler] Register thread %s\n", asCString(_threadType));
        else
            VG_DEBUGPRINT("[Profiler] Register thread %s %2u/%-2u\n", asCString(_threadType), _index, _count);

        // Set thread name for debug
        SetThreadDescription(GetCurrentThread(), core::wstring_convert((string)_name).c_str());

        // Register for profiler markers
        Kernel::getProfiler()->RegisterProfilerThread(_name.c_str());

        // Record
        m_registeredThreads.insert(std::pair(threadId, ThreadInfo(ThreadType((int)_threadType + _index), _name)));
    }

    //--------------------------------------------------------------------------------------
    Scheduler::Scheduler() :
        m_schd(new px_sched::Scheduler())
    {
        px_sched::SchedulerParams params;
                                  params.max_running_threads = static_cast<uint16_t>(std::thread::hardware_concurrency());
                                  params.num_threads = params.max_running_threads;
                                  params.thread_sleep_on_idle_in_microseconds = 0;
        m_schd->init(params);

        // save thread count
        m_threadCount = params.num_threads;
    }

    //--------------------------------------------------------------------------------------
    Scheduler::~Scheduler()
    {
        VG_SAFE_DELETE(m_schd);
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::Start(Job * _job, JobSync * _sync)
    {
        VG_ASSERT(nullptr != _job);
        if (nullptr != _job)
        {
            px_sched::Job job{ _job };
            m_schd->run(std::move(job), reinterpret_cast<px_sched::Sync *>(_sync));
        }
    }

    //--------------------------------------------------------------------------------------
    JobSync Scheduler::Start(Job * _job)
    {
        px_sched::Sync s;
        px_sched::Job job{ _job };
        m_schd->run(std::move(job), &s);
        return *reinterpret_cast<JobSync *>(&s);
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::StartAfter(JobSync * _trigger, Job * _job, JobSync * _sync)
    {
        VG_ASSERT(nullptr != _job);
        if (nullptr != _job)
        {
            px_sched::Job job{ _job };
            m_schd->runAfter(*reinterpret_cast<px_sched::Sync *>(_trigger), std::move(job), reinterpret_cast<px_sched::Sync *>(_sync));
        }
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::Wait(JobSync * _sync)
    {
        m_schd->waitFor(*reinterpret_cast<px_sched::Sync *>(_sync));
    }

    //--------------------------------------------------------------------------------------
    // This cannot safely be done at init even by running jobs, because we cannot be sure 
    // that all the workers will be used and that no job is executed on the main thread etc... 
    // So instead we lazily register worker threads once using TLS check in 'Job::RegisterAndRun'
       //--------------------------------------------------------------------------------------
    void Scheduler::RegisterWorkerThreads()
    {
     
    }

    //--------------------------------------------------------------------------------------
    core::uint Scheduler::GetWorkerThreadCount() const
    {
        return m_threadCount;
    }


    //--------------------------------------------------------------------------------------
    const ThreadInfo * Scheduler::getCurrentThreadInfo() const
    {
        const ThreadID threadId = GetCurrentThreadID();
        return getThreadInfo(threadId);
    }

    //--------------------------------------------------------------------------------------
    const ThreadInfo * Scheduler::getThreadInfo(ThreadID _threadID) const
    {
        core::lock_guard lock(m_registerThreadMutex);

        auto it = m_registeredThreads.find(_threadID);
        if (it != m_registeredThreads.end())
        {
            return &it->second;
        }
        else
        {
            VG_ASSERT(it != m_registeredThreads.end(), "Thread 0x%08X is not registered\n", _threadID);
            return nullptr;
        }
    }

    //--------------------------------------------------------------------------------------
    ThreadType Scheduler::GetCurrentThreadType() const
    {
        if (const ThreadInfo * threadInfo = getCurrentThreadInfo())
            return threadInfo->m_type;
        else
            return ThreadType::Unknown;
    }

    //--------------------------------------------------------------------------------------
    const string & Scheduler::GetCurrentThreadName() const
    {
        static string unknown = "Unknown";

        if (const ThreadInfo * threadInfo = getCurrentThreadInfo())
            return threadInfo->m_name;
        else
            return unknown;
    }

    //--------------------------------------------------------------------------------------
    ThreadType Scheduler::GetThreadType(ThreadID _threadID) const
    {
        if (const ThreadInfo * threadInfo = getThreadInfo(_threadID))
            return threadInfo->m_type;
        else
            return ThreadType::Unknown;
    }

    //--------------------------------------------------------------------------------------
    const string & Scheduler::GetThreadName(ThreadID _threadID) const
    {
        static string unknown = "Unknown";

        if (const ThreadInfo * threadInfo = getThreadInfo(_threadID))
            return threadInfo->m_name;
        else
            return unknown;
    }

    //--------------------------------------------------------------------------------------
    bool Scheduler::IsMainThread() const
    {
        if (const ThreadInfo * threadInfo = getCurrentThreadInfo())
            return ThreadType::Main == threadInfo->m_type;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    bool Scheduler::IsLoadingThread() const 
    {
        if (const ThreadInfo * threadInfo = getCurrentThreadInfo())
            return ThreadType::Loading == threadInfo->m_type;
        else
            return false;
    }
    //--------------------------------------------------------------------------------------
    bool Scheduler::IsWorkerThread() const
    {
        if (const ThreadInfo * threadInfo = getCurrentThreadInfo())
            return ThreadType::Worker == threadInfo->m_type;
        else
            return false;
    }
}