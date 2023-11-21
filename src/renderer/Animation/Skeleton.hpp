#include "Skeleton.h"

//#if !VG_ENABLE_INLINE
//#include "Skeleton.inl"
//#endif

using namespace vg::core;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(Skeleton);

    //--------------------------------------------------------------------------------------
    bool Skeleton::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Skeleton, "Skeleton", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Skeleton::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

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