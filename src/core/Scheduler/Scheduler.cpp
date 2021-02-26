#include "core/Precomp.h"
#include "Scheduler.h"
#include "Job.h"
#include "core/IProfiler.h"

// Job definition must be done before including px_sched.h 
#define PX_SCHED_CUSTOM_JOB_DEFINITION
namespace px_sched 
{
    struct Job 
    {
        vg::core::Job * m_job = nullptr;
        void operator()() { m_job->run(); }
    };
} // px namespace

#define PX_SCHED_IMPLEMENTATION 1
#include "px/px_sched.h"

namespace vg::core
{
    class TestJob : public Job
    {
    public:
        TestJob(const string & _name) :
            Job(_name)
        {

        }

        void run() override
        {
            VG_PROFILE_CPU(Scheduler::getCurrentThreadName().c_str());
            //VG_DEBUGPRINT("Job \"%s\" running on \"%s\"\n", getName().c_str(), Scheduler::getCurrentThreadName().c_str());
            
            static volatile uint test = 0;
            uint r = rand() & 0x0FFFF;
            for (uint i = 0; i < (r<<4); ++i)
                test += i;
        }

    private:
        char m_name[256];
    };

    class RegisterProfilerThreadJob : public Job
    {
    public:
        RegisterProfilerThreadJob(core::uint _index) : 
            Job(""),
            m_index(_index)
        {

        }

        void run() override
        {
            Kernel::getProfiler()->registerThread(Scheduler::getCurrentThreadName().c_str());
        }

    private:
        core::uint m_index;
    };

    //--------------------------------------------------------------------------------------
    Scheduler::Scheduler() :
        m_schd(new px_sched::Scheduler())
    {
        px_sched::SchedulerParams params;
                                  params.thread_sleep_on_idle_in_microseconds = 0;
        m_schd->init(params);

        registerProfilerThreads(params.num_threads);
    }

    //--------------------------------------------------------------------------------------
    Scheduler::~Scheduler()
    {
        VG_SAFE_DELETE(m_schd);
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::registerProfilerThreads(core::uint _count)
    {
        px_sched::Sync s;
        for (uint i = 0; i < _count*_count; ++i)
        {
            px_sched::Job job{ new RegisterProfilerThreadJob(i) };
            m_schd->run(job, &s);
        }
        m_schd->waitFor(s);
    }

    //--------------------------------------------------------------------------------------
    const string Scheduler::getCurrentThreadName()
    {
        return px_sched::Scheduler::current_thread_name();
    }

    //--------------------------------------------------------------------------------------
    void Scheduler::test()
    {
        // Sync objects can be used later to wait for them
        px_sched::Sync s;

        VG_PROFILE_CPU("SchedulerTest");
      
        for (uint i = 0; i < 32; ++i)
        {
            char jobName[256];
            sprintf_s(jobName, "Job %u", i);

            px_sched::Job job{ new TestJob(jobName) };
            m_schd->run(job, &s);
        }

        // wait for all tasks to finish
        {
            VG_PROFILE_CPU("SyncJobs");
            m_schd->waitFor(s); 
        }
    }
}