#include "MaterialModelType.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(MaterialModelType, "Material Model Type");

    //--------------------------------------------------------------------------------------
    bool MaterialModelType::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(MaterialModelType, m_shader, "Shader");
        setPropertyDescription(MaterialModelType, m_shader, "The shader this material will use");

        return true;
    }
}