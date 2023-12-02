#include "ObjectFilter.h"

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool ObjectFilter::ShouldCollide(JPH::ObjectLayer _objectLayer1, JPH::ObjectLayer _objectLayer2) const
    {
        physics::ObjectLayer obj1 = (physics::ObjectLayer)_objectLayer1;
        physics::ObjectLayer obj2 = (physics::ObjectLayer)_objectLayer2;

        switch (obj1)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(obj1);
                return false;

            case ObjectLayer::NonMoving:
                return obj2 == ObjectLayer::Moving; // Non moving only collides with moving

            case ObjectLayer::Moving:
                return true; // Moving collides with everything
        }
    }
}