#pragma once
#include "core/Scheduler/Job.h"

namespace vg::engine
{
    class MeshComponent;

    class AnimationJob : public core::Job
    {
    public:
        const char * GetClassName() const final { return "UpdateSkeletonJob"; }

        AnimationJob(MeshComponent * _meshComponent);
        void Run() override;

    private:
        MeshComponent * m_meshComponent;
    };
}