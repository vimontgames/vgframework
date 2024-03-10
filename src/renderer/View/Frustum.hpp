#include "Frustum.h"
#include "core/Misc/AABB/AABB.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Local AABB x world vs. Frustum
    //--------------------------------------------------------------------------------------
    FrustumTest Frustum::intersects(const core::AABB & _aabb, const core::float4x4 & _world) const
    {
        FrustumTest result = FrustumTest::Inside;
        core::AABB worldAABB(_aabb, _world);

        for (uint i = 0; i < countof(planes); ++i)
        {
            float4 plane = planes[i];

            float3 cmp = plane.xyz < 0.0f;

            float3 vmin = select(cmp, worldAABB.m_min, worldAABB.m_max);
            float3 vmax = select(cmp, worldAABB.m_max, worldAABB.m_min);
 
            if (dot(plane.xyz, vmin) < -plane.w)
                return FrustumTest::Outside;

            if (dot(plane.xyz, vmax) <= -plane.w)
                result = FrustumTest::Intersect;
        }

        return result;
    }

    //--------------------------------------------------------------------------------------
    // Local sphere x world vs. Frustum
    //--------------------------------------------------------------------------------------
    FrustumTest Frustum::intersects(float _radius, const core::float4x4 & _world) const
    {
        FrustumTest result = FrustumTest::Inside;
        float3 center = _world[3].xyz;

        for (uint i = 0; i < countof(planes); ++i)
        {
            float4 plane = planes[i];

            float dist = dot(float4(center.xyz, 1.0f), plane);

            if (dist < -_radius)
                return FrustumTest::Outside;

            if (dist <= _radius)
                result = FrustumTest::Intersect;
        }

        return result;
    }
}