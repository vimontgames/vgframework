#pragma once
#include "Physics/Physics_Consts.h"

namespace vg::physics
{
    class BroadPhaseLayer final : public JPH::BroadPhaseLayerInterface
    {
        public:
                                BroadPhaseLayer         ();

        JPH::uint               GetNumBroadPhaseLayers  () const final override;
        JPH::BroadPhaseLayer    GetBroadPhaseLayer      (JPH::ObjectLayer _layer) const final override;

        #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        const char *            GetBroadPhaseLayerName  (JPH::BroadPhaseLayer _layer) const final override;
        #endif

    private:
        BPLayer                 m_objectToBroadPhase[core::enumCount<physics::ObjectLayer>()];
    };
}