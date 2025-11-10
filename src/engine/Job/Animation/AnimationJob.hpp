#include "Animation/AnimationJob.h"
#include "engine/Component/Renderer/Instance/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    AnimationJob::AnimationJob(MeshComponent * _meshComponent) :
        core::Job("Animation", _meshComponent),
        m_meshComponent(_meshComponent)
    {

    }

    //--------------------------------------------------------------------------------------
    void AnimationJob::Run()
    {
        auto * instance = m_meshComponent->getMeshInstance();
        instance->UpdateSkeleton();
    }
}