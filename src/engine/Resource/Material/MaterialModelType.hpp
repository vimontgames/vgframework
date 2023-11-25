#include "MaterialModelType.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialModelType);

    //--------------------------------------------------------------------------------------
    bool MaterialModelType::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialModelType, "Material Shader", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialModelType::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerProperty(MaterialModelType, m_shader, "Shader");

        return true;
    }
}