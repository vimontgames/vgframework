#pragma once
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    // Class that determines if an object layer can collide with a broadphase layer
    //--------------------------------------------------------------------------------------
    class BroadPhaseFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
    {
        bool ShouldCollide(JPH::ObjectLayer _objectLayer, JPH::BroadPhaseLayer _broadphaseLayer) const final override;
    };
}