#include "physics/Precomp.h"
#include "JobSystemAdapter.h"
#include "core/Kernel.h"
#include "core/Scheduler/Scheduler.h"
#include "core/Scheduler/Job.h"

#include "PhysicsJobs.hpp"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    JobSystemAdapter::JobSystemAdapter(core::uint _maxPhysicsBarriers) : 
        JPH::JobSystemWithBarrier(_maxPhysicsBarriers)
    {
        
    }

    //--------------------------------------------------------------------------------------
    JobSystemAdapter::~JobSystemAdapter()
    {

    }

    //--------------------------------------------------------------------------------------
    vg::core::Scheduler * JobSystemAdapter::getScheduler() const
    {
        return (vg::core::Scheduler*)vg::core::Kernel::getScheduler();
    }

    //--------------------------------------------------------------------------------------
    // Get maximum number of concurrently executing jobs
    //--------------------------------------------------------------------------------------
    int JobSystemAdapter::GetMaxConcurrency() const
    {
        return getScheduler()->GetWorkerThreadCount();
    }

    //--------------------------------------------------------------------------------------
    // Create a new job, the job is started immediately if inNumDependencies == 0 otherwise it starts when RemoveDependency causes the dependency counter to reach 0.
    //--------------------------------------------------------------------------------------
    JobSystemAdapter::JobHandle JobSystemAdapter::CreateJob(const char * _name, JPH::ColorArg _color, const JobFunction & _jobFunction, JPH::uint32 _numDependencies)
    {
        #pragma push_macro("new")
        #undef new
        auto * jphJob = new JPH::JobSystem::Job(_name, _color, this, _jobFunction, _numDependencies);
        #pragma pop_macro("new")

        // Construct handle to keep a reference, the job is queued below and may immediately complete
        JobHandle handle(jphJob);
        
        // If there are no dependencies, queue the job now
        if (0 == _numDependencies)
            QueueJob(jphJob);
        
        // Return the handle
        return handle;
    }

    //--------------------------------------------------------------------------------------
    // Adds a job to the job queue
    //--------------------------------------------------------------------------------------
    void JobSystemAdapter::QueueJob(JobSystemAdapter::Job * _job)
    {
        // TODO: make pool for jobs
        auto * vgJob = new PhysicsJob(_job);
        getScheduler()->Start(vgJob);
    }

    //--------------------------------------------------------------------------------------
    // Adds a number of jobs at once to the job queue
    //--------------------------------------------------------------------------------------
    void JobSystemAdapter::QueueJobs(JobSystemAdapter::Job ** _jobs, JPH::uint _numJobs)
    {
        for (core::uint i = 0; i < _numJobs; ++i)
            QueueJob(_jobs[i]);
    }

    //--------------------------------------------------------------------------------------
    // Frees a job
    //--------------------------------------------------------------------------------------
    void JobSystemAdapter::FreeJob(JobSystemAdapter::Job * _job)
    {
        VG_SAFE_DELETE(_job);
    }
}