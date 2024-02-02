#pragma once
#include "physics/Physics_Consts.h"

namespace vg::physics
{
    class BroadPhaseLayer final : public JPH::BroadPhaseLayerInterface
    {
        public:
                                BroadPhaseLayer         ();

        JPH::uint               GetNumBroadPhaseLayers  () const final override;
        JPH::BroadPhaseLayer    GetBroadPhaseLayer      (JPH::ObjectLayer _layer) const final override;
        const char *            GetBroadPhaseLayerName  (JPH::BroadPhaseLayer _layer) const final override;

    private:
        BPLayer                 m_objectToBroadPhase[core::enumCount<physics::ObjectLayer>()];
    };
}