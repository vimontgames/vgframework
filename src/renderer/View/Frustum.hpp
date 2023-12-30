#include "Frustum.h"
#include "renderer/Geometry/AABB/AABB.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    FrustumTest Frustum::intersects(const AABB & _aabb, const core::float4x4 & _world) const
    {
        AABB worldAABB(_aabb, _world);

        FrustumTest result = FrustumTest::Inside;

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
}