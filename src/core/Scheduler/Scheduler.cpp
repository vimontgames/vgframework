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
        void operator()() { m_job->Run(); }
    };
} // px namespace

#define PX_SCHED_IMPLEMENTATION 1
#pragma push_macro("new")
#undef new
#include "px/px_sched.h"
#pragma pop_macro("new")


namespace vg::core
{
    class RegisterWorkerThreadJob : public Job
    {
    public:
        const char * GetClassName() const final { return "RegisterWorkerThreadJob"; }

        RegisterWorkerThreadJob(const string & _name, IObject * _parent = nullptr) :
            Job(_name, _parent)
        {

        }

        void Run() override
        {
            // User name from scheduler to make sure they match (they seem valid at this point)
            const auto & name = px_sched::Scheduler::current_thread_name(); //  Scheduler::getCurrentThreadName();
            ((Scheduler*)Kernel::getScheduler())->RegisterCurrentThread(name);
        }

    private:
    };

    //--------------------------------------------------------------------------------------
    ThreadID Scheduler::GetCurrentThreadID() const
    {
        return std::this_thread::get_id();
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::RegisterCurrentThread(const core::string & _name)
    {
        core::lock_guard<mutex> lock(m_registerThreadMutex);

        // Current thread id
        ThreadID threadId = GetCurrentThreadID();

        // Check if already registered
        if (m_registeredThreads.end() != m_registeredThreads.find(threadId))
            return;  

        VG_INFO("[Profiler] Register Thread \"%s\" (0x%08X)", _name.c_str(), threadId);

        // Set thread name for debug
        SetThreadDescription(GetCurrentThread(), core::wstring_convert((string)_name).c_str());

        // Register for profiler markers
        Kernel::getProfiler()->registerProfilerThread(_name.c_str());

        // Record
        m_registeredThreads.insert(std::pair(threadId, _name));
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
            m_schd->run(job, reinterpret_cast<px_sched::Sync *>(_sync));
        }
    }

    //--------------------------------------------------------------------------------------
    JobSync Scheduler::Start(Job * _job)
    {
        px_sched::Sync s;
        px_sched::Job j{ _job };
        m_schd->run(j, &s);
        return *reinterpret_cast<JobSync *>(&s);
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::StartAfter(JobSync * _trigger, Job * _job, JobSync * _sync)
    {
        VG_ASSERT(nullptr != _job);
        if (nullptr != _job)
        {
            px_sched::Job job{ _job };
            m_schd->runAfter(*reinterpret_cast<px_sched::Sync *>(_trigger), job, reinterpret_cast<px_sched::Sync *>(_sync));
        }
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::Wait(JobSync * _sync)
    {
        m_schd->waitFor(*reinterpret_cast<px_sched::Sync *>(_sync));
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::RegisterWorkerThreads()
    {
        RegisterWorkerThreadJob registerThreadJob("RegisterWorkerThreads", nullptr);
        px_sched::Sync s;
        for (uint i = 0; i < m_threadCount; ++i)
        {
            px_sched::Job job{ &registerThreadJob };
            m_schd->run(job, &s);
        }
        m_schd->waitFor(s);
    }

    //--------------------------------------------------------------------------------------
    core::uint Scheduler::GetWorkerThreadCount() const
    {
        return m_threadCount;
    }

    //--------------------------------------------------------------------------------------
    const string Scheduler::GetCurrentThreadName() const
    {
        const ThreadID threadId = GetCurrentThreadID();

        auto it = m_registeredThreads.find(threadId);
        if (it != m_registeredThreads.end())
        {
            return it->second;
        }
        else
        {
            VG_ASSERT(it != m_registeredThreads.end(), "Thread 0x%08X is not registered\n", threadId);
            return {};
        }
    }
}