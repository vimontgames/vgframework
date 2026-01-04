#include "core/Precomp.h"
#include "Job.h"
#include "Scheduler.h"

#pragma push_macro("new")
#undef new
#include "px/px_sched.h"
#pragma pop_macro("new")

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Job::Job(const string & _name, IObject * _parent) :
        Object(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Job::~Job()
    {

    }

    //--------------------------------------------------------------------------------------
    // Auto-register thread when running from a worker
    //--------------------------------------------------------------------------------------
    void Job::RegisterAndRun()
    {
        thread_local bool registered = false;

        if (!registered)
        {
            // User name from scheduler to make sure they match
            const auto & name = px_sched::Scheduler::current_thread_name();

            // Extract worker index
            std::string input(name);
            if (input._Starts_with("Worker"))
            {
                size_t pos = input.find_last_of('-');
                uint index = -1;
                if (pos != string::npos)
                    index = stoi(input.substr(pos + 1));

                auto * scheduler = ((Scheduler *)Kernel::getScheduler());
                scheduler->RegisterCurrentThread(name, ThreadType::Worker, index, scheduler->GetWorkerThreadCount());
            }

            registered = true;
        }

        Run();
    }
}