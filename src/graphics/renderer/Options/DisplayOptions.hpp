#include "DisplayOptions.h"
#include "core/Object/AutoRegisterClass.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(DisplayOptions);

    //--------------------------------------------------------------------------------------
    DisplayOptions::DisplayOptions(const core::string & _name, core::IObject * _parent) :
        Object(_name, _parent)
    {
        load(this);
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerClass(IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassSingletonHelper(DisplayOptions, "Display Options", IObjectDescriptor::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::load(IObject * _object)
    {
        return _object->loadFromFile("displayoptions.ini");
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::save(IObject * _object)
    {
        return _object->saveToFile("displayoptions.ini");
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(DisplayOptions, m_toolMode,   "Tool mode",   IPropertyDescriptor::Flags::None);

        _desc.registerPropertyHelper(DisplayOptions, m_opaque,     "Opaque",      IPropertyDescriptor::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_wireframe,  "Wireframe",   IPropertyDescriptor::Flags::SameLine);

        _desc.registerPropertyHelper(DisplayOptions, m_normalMaps, "Normal Maps", IPropertyDescriptor::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_albedoMaps, "Albedo Maps", IPropertyDescriptor::Flags::SameLine);

        static const char * enumValues =
        {
            "Default\0"
            "MatID\0"
            "VSNormal\0"
            "VSTangent\0"
            "VSBinormal\0"
            "VSColor\0"
            "UV0\0"
            "UV1\0"
            "Albedo\0"
            "Normal\0"
            "\0"
        };

		_desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::Default, "Debug Display", enumValues, IPropertyDescriptor::Flags::None);

        _desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background", IPropertyDescriptor::Flags::Color);

        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IPropertyDescriptor::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IPropertyDescriptor::Flags::SameLine);

        return true;
    }
}