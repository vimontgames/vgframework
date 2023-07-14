#include "DisplayOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(DisplayOptions);

    static const char * filename = "DisplayOptions.xml";

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
        const auto * factory = Kernel::getFactory();
        return factory->loadFromXML(_object, filename);
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::save(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        return factory->saveToXML(_object, filename);
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

        EnumHelper<DisplayMode> displayModeEnum;
        _desc.registerPropertyEnum(DisplayOptions, DisplayMode, m_debugDisplayMode, "Debug Display", IProperty::Flags::None);

        _desc.registerPropertyHelper(DisplayOptions, m_backgroundColor, "Background", IProperty::Flags::Color);

        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IProperty::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void DisplayOptions::onPropertyChanged(const core::IProperty & _prop)
    {

    }
}