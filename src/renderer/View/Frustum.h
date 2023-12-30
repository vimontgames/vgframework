#pragma once

namespace vg::renderer
{
    class AABB;

    enum class FrustumPlane
    {
        Left = 0,
        Right,
        Top,
        Bottom,
        Near,
        Far
    };

    enum class FrustumTest
    {
        Intersect = 0,
        Inside,
        Outside
    };

    struct Frustum
    {
        core::float4 planes[core::enumCount<FrustumPlane>()];

        FrustumTest intersects(const AABB & _aabb, const core::float4x4 & _world) const;
    };
}