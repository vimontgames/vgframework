#include "BroadPhaseFilter.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool BroadPhaseFilter::ShouldCollide(JPH::ObjectLayer _objectLayer, JPH::BroadPhaseLayer _broadphaseLayer) const
    {
        auto objLayer = (physics::ObjectLayer)_objectLayer;
        auto BPLayer = (physics::BPLayer)(JPH::BroadPhaseLayer::Type)_broadphaseLayer;

        switch (objLayer)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(objLayer);
                return false;

            case ObjectLayer::NonMoving:
                return BPLayer::Moving == BPLayer;

            case ObjectLayer::Moving:
                return true;
        }
    }
}