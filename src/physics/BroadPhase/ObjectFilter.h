#pragma once
#include "Jolt/Physics/Collision/ObjectLayer.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    // Class that determines if two object layers can collide
    //--------------------------------------------------------------------------------------
    class ObjectFilter final : public JPH::ObjectLayerPairFilter
    {
        bool ShouldCollide(JPH::ObjectLayer _objectLayer1, JPH::ObjectLayer _objectLayer2) const final override;
    };
}