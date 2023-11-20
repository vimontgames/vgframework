#pragma once

#include "renderer/ISkeletalAnimation.h"
#include "renderer/Importer/MeshImporterData.h"

namespace vg::renderer
{
    class SkeletalAnimation : ISkeletalAnimation
    {
    public:
        VG_CLASS_DECL(SkeletalAnimation, ISkeletalAnimation)

    private:
        AnimImporterData m_animData;    
    };
}