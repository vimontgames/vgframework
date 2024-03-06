#pragma once

namespace vg::core
{
    class AABB;
}

namespace vg::renderer
{
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

        FrustumTest intersects(const core::AABB & _aabb, const core::float4x4 & _world) const;
        FrustumTest intersects(float _radius, const core::float4x4 & _world) const;
    };
}