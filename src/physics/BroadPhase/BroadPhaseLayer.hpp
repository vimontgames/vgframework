#include "BroadPhaseLayer.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    BroadPhaseLayer::BroadPhaseLayer()
    {
        m_objectToBroadPhase[core::asInteger(physics::ObjectLayer::NonMoving)] = physics::BPLayer::NonMoving;
        m_objectToBroadPhase[core::asInteger(physics::ObjectLayer::Moving)] = physics::BPLayer::NonMoving;
    }

    //--------------------------------------------------------------------------------------
    JPH::uint BroadPhaseLayer::GetNumBroadPhaseLayers() const
    {
        return (JPH::uint)core::enumCount<physics::BPLayer>();
    }

    //--------------------------------------------------------------------------------------
    JPH::BroadPhaseLayer BroadPhaseLayer::GetBroadPhaseLayer(JPH::ObjectLayer _layer) const
    {
        VG_ASSERT(_layer < core::enumCount<physics::ObjectLayer>());
        return getJoltBroadPhaseLayer(m_objectToBroadPhase[(JPH::ObjectLayer)_layer]);
    }

    //--------------------------------------------------------------------------------------
    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char * BroadPhaseLayer::GetBroadPhaseLayerName(JPH::BroadPhaseLayer _layer) const
    {
        return core::asCString((physics::BPLayer)(JPH::BroadPhaseLayer::Type)_layer);
    }
    #endif
}