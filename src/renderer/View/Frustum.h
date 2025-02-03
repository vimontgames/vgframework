#pragma once

namespace vg::core
{
    class AABB;
    class IWorld;
}

vg_enum_class_ns(vg::renderer, FrustumPlane, core::u8,
    Left = 0,
    Right,
    Top,
    Bottom,
    Near,
    Far
);

vg_enum_class_ns(vg::renderer, FrustumTest, core::u8,
    Intersect = 0,
    Inside,
    Outside
);

vg_enum_class_ns(vg::renderer, FrustumCorner, core::u8,
    NearBottomLeft = 0,
    NearBottomRight,
    NearTopLeft,
    NearTopRight,
    FarBottomLeft,
    FarBottomRight,
    FarTopLeft,
    FarTopRight
);

namespace vg::renderer
{
    class Frustum
    {
    public:
        using Planes = core::float4[core::enumCount<FrustumPlane>()];
        using Corners = core::float3[core::enumCount<FrustumCorner>()];

        const Planes & getPlanes() const { return m_planes; }
        const Corners & getCorners() const { return m_corners; }

        void compute(const float4x4 & viewProj);

        FrustumTest intersects(const core::AABB & _aabb, const core::float4x4 & _world) const;
        FrustumTest intersects(float _radius, const core::float4x4 & _world) const;

        void draw(const core::IWorld * _world, core::u32 _color);

    private:
        Planes m_planes;
        Corners m_corners;
    };
}