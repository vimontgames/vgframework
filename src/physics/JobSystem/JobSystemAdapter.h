#pragma once
#include "core/Scheduler/Job.h"

namespace vg::core
{
    class Scheduler;
}

namespace vg::physics
{
    class JobSystemAdapter final : public JPH::JobSystemWithBarrier
    {
    public:
        using Job = JPH::JobSystem::Job;

                                JobSystemAdapter    (core::uint _maxPhysicsBarriers);
                                ~JobSystemAdapter   ();

        const char *            getClassName        () const { return "JobSystemAdapter"; }

        int                     GetMaxConcurrency   () const final override;
        JPH::JobHandle          CreateJob           (const char * _name, JPH::ColorArg _color, const JobFunction & _jobFunction, JPH::uint32 _numDependencies = 0)  final override;
        void                    QueueJob            (JobSystemAdapter::Job * _job)  final override;
        void                    QueueJobs           (JobSystemAdapter::Job ** _jobs, JPH::uint _numJobs)  final override;
        void                    FreeJob             (JobSystemAdapter::Job * _job)  final override;

    private:
        vg::core::Scheduler *   getScheduler        () const;
    };
}