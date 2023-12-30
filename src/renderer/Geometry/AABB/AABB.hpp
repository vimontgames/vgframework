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

        // Boost scale so that the bounding box of a box is visible
        float s = 0.5f * 1.001f;

        float4x4 matLS = float4x4(
            float4(size.x * s, 0, 0, 0),
            float4(0, size.y * s, 0, 0),
            float4(0, 0, size.z * s, 0),
            float4(center * 0.5f, 1)
        );
        return matLS;
    }

    //--------------------------------------------------------------------------------------
    AABB::AABB(const AABB & _other, const float4x4 & _world)
    {
        float4 p000 = mul(float4(_other.m_min.xyz, 1.0f), _world);
        float4 p001 = mul(float4(_other.m_min.xy, _other.m_max.z , 1.0f), _world);
        float4 p010 = mul(float4(_other.m_min.x,  _other.m_max.y, _other.m_min.z, 1.0f), _world);
        float4 p011 = mul(float4(_other.m_min.x,  _other.m_max.yz, 1.0f), _world);
        float4 p100 = mul(float4(_other.m_max.x,  _other.m_min.yz, 1.0f), _world);
        float4 p101 = mul(float4(_other.m_max.x,  _other.m_min.y, _other.m_max.z, 1.0f), _world);
        float4 p110 = mul(float4(_other.m_max.xy, _other.m_min.z, 1.0f), _world);
        float4 p111 = mul(float4(_other.m_max.xyz, 1.0f), _world);

        m_min = min(min(min(p000, p001), min(p010, p011)), min(min(p100, p101), min(p110, p111))).xyz;
        m_max = max(max(max(p000, p001), max(p010, p011)), max(max(p100, p101), max(p110, p111))).xyz;
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