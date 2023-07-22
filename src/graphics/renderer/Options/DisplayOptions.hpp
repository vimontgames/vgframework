#include "DisplayOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"
#include "graphics/renderer/Renderer.h"

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
        if (factory->loadFromXML(_object, filename))
        {
            DisplayOptions * displayOptions = static_cast<DisplayOptions*>(_object);

            auto vSyncProp = _object->getClassDesc()->getPropertyByName("m_VSync");
            displayOptions->ApplyVsync(vSyncProp);

            return true;
        }
        return false;
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

        _desc.registerPropertyEnum(DisplayOptions, graphics::driver::VSync, m_VSync, "VSync");

        _desc.registerPropertyHelper(DisplayOptions, m_toolMode,   "Tool mode");

        _desc.registerPropertyHelper(DisplayOptions, m_opaque,     "Opaque");
        _desc.registerPropertyHelperEx(DisplayOptions, m_wireframe,  "Wireframe",   IProperty::Flags::SameLine);

        _desc.registerPropertyHelper(DisplayOptions, m_normalMaps, "Normal Maps");
        _desc.registerPropertyHelperEx(DisplayOptions, m_albedoMaps, "Albedo Maps", IProperty::Flags::SameLine);

        _desc.registerPropertyEnum(DisplayOptions, DisplayMode, m_debugDisplayMode, "Debug Display");

        _desc.registerPropertyHelperEx(DisplayOptions, m_backgroundColor, "Background", IProperty::Flags::Color);

        // TODO: Move to menu instead
        _desc.registerCallbackHelper(DisplayOptions, load, "Load", IProperty::Flags::None);
        _desc.registerCallbackHelper(DisplayOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void DisplayOptions::onPropertyChanged(const core::IProperty & _prop)
    {
        if (!strcmp(_prop.getName(), "m_VSync"))
            ApplyVsync(&_prop);
    }

    //--------------------------------------------------------------------------------------
    void DisplayOptions::ApplyVsync(const core::IProperty * _prop)
    {
        if (nullptr != _prop)
        {
            auto value = *_prop->GetPropertyEnum<driver::VSync>(this);
            Renderer::get()->SetVSync(value);
        }
    }
}