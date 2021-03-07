#include "DisplayOptions.h"
#include "core/Object/ObjectFactory.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerClass(core::IObjectFactory & _factory)
    {
        core::IObjectDescriptor & desc = _factory.registerClassHelper(DisplayOptions);

        desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background color");
        desc.registerPropertyHelper(DisplayOptions, m_displayNormals, "Display normals");

        return true;
    }
}