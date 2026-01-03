#include "physics/Precomp.h"

#ifdef JPH_DEBUG_RENDERER
#include "DebugRenderer.h"
#include "physics/Physics.h"
#include "renderer/IDebugDraw.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawLine(JPH::RVec3Arg _inFrom, JPH::RVec3Arg _inTo, JPH::ColorArg _inColor)
    {
        renderer::IDebugDraw * debugDraw = Physics::get()->getDebugDraw();

        debugDraw->AddLine(m_world, fromJoltVec3(_inFrom), fromJoltVec3(_inTo), _inColor.GetUInt32());
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawTriangle(JPH::RVec3Arg _inV1, JPH::RVec3Arg _inV2, JPH::RVec3Arg _inV3, JPH::ColorArg _inColor, ECastShadow _inCastShadow)
    {
        // TODO
    }

    //--------------------------------------------------------------------------------------
    void DebugRenderer::DrawText3D(JPH::RVec3Arg _inPosition, const JPH::string_view & _inString, JPH::ColorArg _inColor, float _inHeight)
    {
        // TODO
    }
}
#endif // JPH_DEBUG_RENDERER