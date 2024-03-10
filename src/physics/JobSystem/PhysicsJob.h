#pragma once

namespace vg::physics
{
    class PhysicsJob : public vg::core::Job
    {
    public:
        const char * GetClassName() const final { return "PhysicsJob"; }

        PhysicsJob(JobSystemAdapter::Job * _jphJob);
        ~PhysicsJob();

        void run() override;

    private:
        JobSystemAdapter::Job * m_jphJob = nullptr;
    };
}