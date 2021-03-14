#include "DisplayOptions.h"
#include "core/Object/ObjectFactory.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    DisplayOptions::DisplayOptions()
    {
        setName("DefaultDisplayOptions");
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerClass(IObjectFactory & _factory)
    {
        core::IObjectDescriptor & desc = _factory.registerClassHelper(DisplayOptions);

        registerProperties(desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background color", IPropertyDescriptor::Flags::Color);
        _desc.registerPropertyHelper(DisplayOptions, m_displayNormals, "Display normals", IPropertyDescriptor::Flags::None);

        return true;
    }
}