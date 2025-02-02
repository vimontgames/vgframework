#include "Frustum.h"
#include "core/Misc/AABB/AABB.h"
#include "renderer/DebugDraw/DebugDraw.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void Frustum::compute(const float4x4 & _viewProj)
    {
        m_planes[asInteger(FrustumPlane::Left)].x = _viewProj._m03 + _viewProj._m00;
        m_planes[asInteger(FrustumPlane::Left)].y = _viewProj._m13 + _viewProj._m10;
        m_planes[asInteger(FrustumPlane::Left)].z = _viewProj._m23 + _viewProj._m20;
        m_planes[asInteger(FrustumPlane::Left)].w = _viewProj._m33 + _viewProj._m30;

        m_planes[asInteger(FrustumPlane::Right)].x = _viewProj._m03 - _viewProj._m00;
        m_planes[asInteger(FrustumPlane::Right)].y = _viewProj._m13 - _viewProj._m10;
        m_planes[asInteger(FrustumPlane::Right)].z = _viewProj._m23 - _viewProj._m20;
        m_planes[asInteger(FrustumPlane::Right)].w = _viewProj._m33 - _viewProj._m30;

        m_planes[asInteger(FrustumPlane::Top)].x = _viewProj._m03 - _viewProj._m01;
        m_planes[asInteger(FrustumPlane::Top)].y = _viewProj._m13 - _viewProj._m11;
        m_planes[asInteger(FrustumPlane::Top)].z = _viewProj._m23 - _viewProj._m21;
        m_planes[asInteger(FrustumPlane::Top)].w = _viewProj._m33 - _viewProj._m31;

        m_planes[asInteger(FrustumPlane::Bottom)].x = _viewProj._m03 + _viewProj._m01;
        m_planes[asInteger(FrustumPlane::Bottom)].y = _viewProj._m13 + _viewProj._m11;
        m_planes[asInteger(FrustumPlane::Bottom)].z = _viewProj._m23 + _viewProj._m21;
        m_planes[asInteger(FrustumPlane::Bottom)].w = _viewProj._m33 + _viewProj._m31;

        m_planes[asInteger(FrustumPlane::Near)].x = _viewProj._m02;
        m_planes[asInteger(FrustumPlane::Near)].y = _viewProj._m12;
        m_planes[asInteger(FrustumPlane::Near)].z = _viewProj._m22;
        m_planes[asInteger(FrustumPlane::Near)].w = _viewProj._m32;

        m_planes[asInteger(FrustumPlane::Far)].x = _viewProj._m03 - _viewProj._m02;
        m_planes[asInteger(FrustumPlane::Far)].y = _viewProj._m13 - _viewProj._m12;
        m_planes[asInteger(FrustumPlane::Far)].z = _viewProj._m23 - _viewProj._m22;
        m_planes[asInteger(FrustumPlane::Far)].w = _viewProj._m33 - _viewProj._m32;

        for (uint i = 0; i < countof(m_planes); ++i)
        {
            float normXYZ = rsqrt(dot(m_planes[i].xyz, m_planes[i].xyz));
            m_planes[i] /= normXYZ;
        }

        const float4x4 invViewProj = inverse(_viewProj);

        static const float4 frustumCorner[] =
        {
            { -1, -1, 0, 1 },  // FrustumCornerNearBottomLeft = 0
            {  1, -1, 0, 1 },  // FrustumCornerNearBottomRight,
            { -1,  1, 0, 1 },  // FrustumCornerNearTopLeft,
            {  1,  1, 0, 1 },  // FrustumCornerNearTopRight,
            { -1, -1, 1, 1 },  // FrustumCornerFarBottomLeft,
            {  1, -1, 1, 1 },  // FrustumCornerFarBottomRight,
            { -1,  1, 1, 1 },  // FrustumCornerFarTopLeft,
            {  1,  1, 1, 1 }   // FrustumCornerFarTopRight
        };

        for (int i = 0; i < core::enumCount<FrustumCorner>(); ++i)
        {
            float4 corner = mul(frustumCorner[i], invViewProj);
            m_corners[i] = corner.xyz / corner.w;
        }
    }

    //--------------------------------------------------------------------------------------
    void Frustum::draw(const core::IWorld * _world, core::u32 _color)
    {
        auto * debugDraw = DebugDraw::get();

        debugDraw->AddLine(_world, m_corners[0], m_corners[1], _color);
        debugDraw->AddLine(_world, m_corners[1], m_corners[3], _color);
        debugDraw->AddLine(_world, m_corners[3], m_corners[2], _color);
        debugDraw->AddLine(_world, m_corners[2], m_corners[0], _color);

        debugDraw->AddLine(_world, m_corners[0], m_corners[4], _color);
        debugDraw->AddLine(_world, m_corners[1], m_corners[5], _color);
        debugDraw->AddLine(_world, m_corners[2], m_corners[6], _color);
        debugDraw->AddLine(_world, m_corners[3], m_corners[7], _color);

        debugDraw->AddLine(_world, m_corners[4], m_corners[5], _color);
        debugDraw->AddLine(_world, m_corners[5], m_corners[7], _color);
        debugDraw->AddLine(_world, m_corners[7], m_corners[6], _color);
        debugDraw->AddLine(_world, m_corners[6], m_corners[4], _color);
    }

    //--------------------------------------------------------------------------------------
    // Local AABB x world vs. Frustum
    //--------------------------------------------------------------------------------------
    FrustumTest Frustum::intersects(const core::AABB & _aabb, const core::float4x4 & _world) const
    {
        FrustumTest result = FrustumTest::Inside;
        core::AABB worldAABB(_aabb, _world);

        for (uint i = 0; i < countof(m_planes); ++i)
        {
            float4 plane = m_planes[i];

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

        for (uint i = 0; i < countof(m_planes); ++i)
        {
            float4 plane = m_planes[i];

            float dist = dot(float4(center.xyz, 1.0f), plane);

            if (dist < -_radius)
                return FrustumTest::Outside;

            if (dist <= _radius)
                result = FrustumTest::Intersect;
        }

        return result;
    }
}