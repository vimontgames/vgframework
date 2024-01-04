#include "Animation/AnimationJob.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    AnimationJob::AnimationJob(MeshComponent * _meshComponent, bool _debugDrawBones) :
        core::Job("Animation", _meshComponent),
        m_meshComponent(_meshComponent),
        m_debugDrawBones(_debugDrawBones)
    {

    }

    //--------------------------------------------------------------------------------------
    void AnimationJob::run()
    {
        //VG_PROFILE_CPU("AnimationJob");

        auto * instance = m_meshComponent->getMeshInstance();
        instance->UpdateSkeleton();

        if (m_debugDrawBones)
        {
            #if 0
            instance->DrawSkeleton();
            #else
            // Done in a separate job sync'ed before start debug draw update
            #endif
        }
    }
}