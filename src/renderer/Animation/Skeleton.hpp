#include "Skeleton.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void Skeleton::setBoneIndices(const core::vector<core::u32> & _bonesIndices)
    {
        m_boneIndices = _bonesIndices;
    }

    //--------------------------------------------------------------------------------------
    void Skeleton::setBoneMatrices(const core::vector<core::float4x4> & _bonesMatrices)
    {
        m_boneMatrices = _bonesMatrices;
    }
}