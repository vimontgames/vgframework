#include "DisplayOptions.h"
#include "core/Object/ObjectFactory.h"
#include "core/File/File.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    DisplayOptions::DisplayOptions()
    {
        load(this);
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerClass(IObjectFactory & _factory)
    {
        core::IObjectDescriptor & desc = _factory.registerClassHelper(DisplayOptions);

        registerProperties(desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::load(IObject * _object)
    {
        const auto * factory = Kernel::getObjectFactory();

        string s;
        if (readFile("displayoptions.ini", s))
            if (factory->serializeFromString(_object, s))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::save(IObject * _object)
    {
        const auto * factory = Kernel::getObjectFactory();

        string s;
        if (factory->serializeToString(s, _object))
            if (writeFile("displayoptions.ini", s))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(DisplayOptions, m_toolMode, "Tool mode", IPropertyDescriptor::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_displayNormals, "Display normals", IPropertyDescriptor::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background color", IPropertyDescriptor::Flags::Color);

        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IPropertyDescriptor::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IPropertyDescriptor::Flags::None);

        return true;
    }
}