#pragma once

#include "core/File/Buffer.h"

namespace vg::renderer
{
    class AABB 
    {
    public:

        AABB(const core::float3 & _min = core::float3(0, 0, 0), const core::float3 & _max = core::float3(0, 0, 0)) :
            m_min(_min),
            m_max(_max)
        {

        }

        VG_INLINE void grow(const core::float3 & _point);

        core::float3 m_min = core::float3(0, 0, 0);
        core::float3 m_max = core::float3(0, 0, 0);

        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;
    };
}

#if VG_ENABLE_INLINE
#include "AABB.inl"
#endif