#pragma once

#ifdef JPH_DEBUG_RENDERER

#pragma push_macro("new")
#undef new
#include "Jolt/Renderer/DebugRendererSimple.h"
#pragma pop_macro("new")  

#include "core/Singleton/Singleton.h"

namespace vg::core
{
    class IWorld;
}

namespace JPH
{
    class Triangle;
}

namespace vg::physics
{
    class DebugRenderer final : public JPH::DebugRendererSimple, public core::Singleton<DebugRenderer>
    {
    public:
        void SetWorld(const core::IWorld * _world) { m_world = _world; }

        void DrawLine(JPH::RVec3Arg _inFrom, JPH::RVec3Arg _inTo, JPH::ColorArg _inColor) final override;
        void DrawTriangle(JPH::RVec3Arg _inV1, JPH::RVec3Arg _inV2, JPH::RVec3Arg _inV3, JPH::ColorArg _inColor, ECastShadow _inCastShadow) final override;
        void DrawText3D(JPH::RVec3Arg _inPosition, const JPH::string_view & _inString, JPH::ColorArg _inColor, float _inHeight) final override;

    private:
        const core::IWorld * m_world = nullptr;
    };
}

#endif