#include "ShapeDesc.h"
#include "physics/Physics.h"

using namespace vg::core;

namespace vg::physics
{
    //--------------------------------------------------------------------------------------
    bool ShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(ShapeDesc, m_offset, "Offset");

        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SphereShapeDesc, "Sphere Shape Settings");
    //--------------------------------------------------------------------------------------
    bool SphereShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(SphereShapeDesc, m_radius, "Radius");
        return true;
    }

    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(BoxShapeDesc, "Box Shape Settings");
    //--------------------------------------------------------------------------------------
    bool BoxShapeDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        registerProperty(BoxShapeDesc, m_size, "Size");
        return true;
    }
}