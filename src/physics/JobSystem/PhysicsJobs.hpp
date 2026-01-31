#include "PhysicsJob.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    PhysicsJob::PhysicsJob(JobSystemAdapter::Job * _jphJob) :
        Job("PhysicsJob", nullptr)
    {
        #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        SetName(_jphJob->GetName());
        #endif

        _jphJob->AddRef();
        m_jphJob = _jphJob;
    }

    //--------------------------------------------------------------------------------------
    PhysicsJob::~PhysicsJob()
    {
        m_jphJob->Release();
    }

    //--------------------------------------------------------------------------------------
    void PhysicsJob::Run()
    {
        #if VG_OBJECT_HAS_STATIC_NAME
        VG_PROFILE_CPU(GetStaticName());
        #else
        VG_PROFILE_CPU(GetName()); // string
        #endif

        m_jphJob->Execute();

        // Destroy job after use
        this->Release();
    }
}