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
        _desc.registerPropertyHelper(DisplayOptions, m_opaque,     "Opaque",      IPropertyDescriptor::Flags::SameLine);
        _desc.registerPropertyHelper(DisplayOptions, m_wireframe,  "Wireframe",   IPropertyDescriptor::Flags::SameLine);

        _desc.registerPropertyHelper(DisplayOptions, m_normalMaps, "Normal Maps", IPropertyDescriptor::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_albedoMaps, "Albedo Maps", IPropertyDescriptor::Flags::SameLine);

		_desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::Default,    "Default",   IPropertyDescriptor::Flags::Radio);
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::MatID,      "MatID",     IPropertyDescriptor::Flags::Radio | IPropertyDescriptor::Flags::SameLine);

		_desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::VSNormal,   "Normal VS", IPropertyDescriptor::Flags::Radio);
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::VSTangent,  "Tangent",   IPropertyDescriptor::Flags::Radio | IPropertyDescriptor::Flags::SameLine);
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::VSBinormal, "Binormal",  IPropertyDescriptor::Flags::Radio | IPropertyDescriptor::Flags::SameLine);
                                                                                                                                
		_desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::UV0,        "UV0",       IPropertyDescriptor::Flags::Radio);
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::UV1,        "UV1",       IPropertyDescriptor::Flags::Radio | IPropertyDescriptor::Flags::SameLine);
                                                                                                                                
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::Albedo,     "Albedo",    IPropertyDescriptor::Flags::Radio);
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, DisplayMode::PSNormal,   "Normal PS", IPropertyDescriptor::Flags::Radio | IPropertyDescriptor::Flags::SameLine);

        _desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background", IPropertyDescriptor::Flags::Color);

        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IPropertyDescriptor::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IPropertyDescriptor::Flags::SameLine);

        return true;
    }
}