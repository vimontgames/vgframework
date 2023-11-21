#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    class Skeleton : public core::Object
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(Skeleton, core::Object)

        void                                            setBoneIndices      (const core::vector<core::u32> & _bonesIndices);
        void                                            setBoneMatrices     (const core::vector<core::float4x4> & _bonesMatrices);

        /*VG_INLINE*/ inline const core::vector<core::u32> &       getBoneIndices      () const;
        /*VG_INLINE*/ inline const core::vector<core::float4x4> &  getBoneMatrices     () const;

    private:
        core::vector<core::u32>                         m_boneIndices;
        core::vector<core::float4x4>                    m_boneMatrices;
    };
}

//#if VG_ENABLE_INLINE
#include "Skeleton.inl"
//#endif