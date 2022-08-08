#include "DisplayOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumDesc.h"

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
    bool DisplayOptions::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassSingletonHelper(DisplayOptions, "Display Options", IClassDesc::Flags::None))
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
    bool DisplayOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(DisplayOptions, m_toolMode,   "Tool mode",   IProperty::Flags::None);

        _desc.registerPropertyHelper(DisplayOptions, m_opaque,     "Opaque",      IProperty::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_wireframe,  "Wireframe",   IProperty::Flags::SameLine);

        _desc.registerPropertyHelper(DisplayOptions, m_normalMaps, "Normal Maps", IProperty::Flags::None);
        _desc.registerPropertyHelper(DisplayOptions, m_albedoMaps, "Albedo Maps", IProperty::Flags::SameLine);

        EnumDesc<DisplayMode> displayModeDesc;
        _desc.registerPropertyEnumHelper(DisplayOptions, DisplayMode, m_debugDisplayMode, "Debug Display", displayModeDesc.getCount(), displayModeDesc.getNames(), displayModeDesc.getValues(), IProperty::Flags::None);

        _desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background", IProperty::Flags::Color);

        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IProperty::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }
}