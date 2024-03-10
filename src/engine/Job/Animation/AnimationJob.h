#pragma once
#include "core/Scheduler/Job.h"

namespace vg::engine
{
    class MeshComponent;

    class AnimationJob : public core::Job
    {
    public:
        const char * GetClassName() const final { return "UpdateSkeletonJob"; }

        AnimationJob(MeshComponent * _meshComponent, bool _debugDrawBones);
        void run() override;

    private:
        MeshComponent * m_meshComponent;
        bool            m_debugDrawBones = false;
    };
}