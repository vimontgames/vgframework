#include "Animation/DrawSkeletonJob.h"
#include "engine/Component/Mesh/MeshComponent.h"
#include "renderer/IMeshInstance.h"
#include "core/GameObject/GameObject.h"

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    DrawSkeletonJob::DrawSkeletonJob(MeshComponent * _meshComponent) :
        core::Job("DrawSkeleton", _meshComponent),
        m_meshComponent(_meshComponent)
    {

    }

    //--------------------------------------------------------------------------------------
    void DrawSkeletonJob::run()
    {
        //VG_PROFILE_CPU("DrawSkeletonJob");

        auto * instance = m_meshComponent->getMeshInstance();
        auto * world = m_meshComponent->getGameObject()->GetWorld();
        instance->DrawSkeleton(world);
    }
}
