#include "AABB.h"

#if !VG_ENABLE_INLINE
#include "AABB.inl"
#endif

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Matrix used to draw AABB using a signed unit cube between {-1,-1,-1} and {+1,+1,+1}
    //--------------------------------------------------------------------------------------
    core::float4x4 AABB::getLocalSpaceUnitCubeMatrix() const
    {
        float3 size = m_max - m_min;
        float3 center = m_max + m_min;

        float4x4 matLS = float4x4(
            float4(size.x * 0.5f, 0, 0, 0),
            float4(0, size.y * 0.5f, 0, 0),
            float4(0, 0, size.z * 0.5f, 0),
            float4(center * 0.5f, 1)
        );
        return matLS;
    }

    //--------------------------------------------------------------------------------------
    bool AABB::read(io::Buffer & _buffer)
    {
        _buffer.read(&m_min);
        _buffer.read(&m_max);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AABB::write(io::Buffer & _buffer) const
    {
        _buffer.write(m_min);
        _buffer.write(m_max);

        return true;
    }
}