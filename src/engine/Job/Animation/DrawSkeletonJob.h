#pragma once
#include "core/Scheduler/Job.h"

namespace vg::engine
{
    class MeshComponent;

    class DrawSkeletonJob : public core::Job
    {
    public:
        const char * getClassName() const final { return "DrawSkeletonJob"; }

        DrawSkeletonJob(MeshComponent * _meshComponent);
        void run() override;

    private:
        MeshComponent * m_meshComponent;
    };
}