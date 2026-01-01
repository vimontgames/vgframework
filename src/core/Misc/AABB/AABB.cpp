#include "core/Precomp.h"
#include "AABB.h"
#include "core/Math/Math.h"

#if !VG_ENABLE_INLINE
#include "AABB.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    void AABB::reset()
    {
        m_min = float3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
        m_max = float3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);
    }

    //--------------------------------------------------------------------------------------
    bool AABB::isFinite() const
    {
        return all(m_min != MAX_FLOAT) && all(m_max != MIN_FLOAT);
    }

    //--------------------------------------------------------------------------------------
    // Matrix used to draw AABB using a signed unit cube between {-1,-1,-1} and {+1,+1,+1}
    //--------------------------------------------------------------------------------------
    core::float4x4 AABB::getLocalSpaceUnitCubeMatrix() const
    {
        float3 size = getSize();
        float3 center = getCenter();

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
        *this = transform(_other, _world);
    }

    //--------------------------------------------------------------------------------------
    // Compute AABB using the "abs dot" trick
    //--------------------------------------------------------------------------------------
    AABB AABB::transform(const AABB & _other, const core::float4x4 & _world)
    {
        AABB aabb;

        float3 c = (_other.m_min + _other.m_max) * 0.5f;
        float3 e = (_other.m_max - _other.m_min) * 0.5f;

        float4 center = mul(float4(c, 1.0f), _world);

        float3 I = _world[0].xyz;
        float3 J = _world[1].xyz;
        float3 K = _world[2].xyz;

        float3 newExtents = abs(I) * e.x + abs(J) * e.y + abs(K) * e.z;
        
        aabb.m_min = center.xyz - newExtents;
        aabb.m_max = center.xyz + newExtents;

        return aabb;
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