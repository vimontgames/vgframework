#include "DisplayOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"
#include "renderer/Renderer.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_AUTO_REGISTER_CLASS(DisplayOptions);

    static const char * filename = "DisplayOptions.xml";

    //--------------------------------------------------------------------------------------
    DisplayOptions::DisplayOptions(const core::string & _name, core::IObject * _parent) :
        Object(_name, _parent),
        m_debugDisplayMode(DisplayMode::Default),
        m_displayFlags(DisplayFlags::AlbedoMap | DisplayFlags::NormalMap)
    {
        load(this);
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassSingletonHelper(DisplayOptions, "DisplayOptions", IClassDesc::Flags::None))
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

        _desc.registerPropertyHelperEx(DisplayOptions, m_backgroundColor, "Background", IProperty::Flags::Color);

        _desc.registerPropertyHelper(DisplayOptions, m_toolMode, "Toolmode");

        _desc.registerPropertyEnum(DisplayOptions, DisplayMode, m_debugDisplayMode, "Display Mode");
        _desc.registerPropertyEnumBitfield(DisplayOptions, DisplayFlags, m_displayFlags, "Display Flags");
        _desc.registerPropertyEnum(DisplayOptions, gfx::VSync, m_VSync, "VSync");

        _desc.registerPropertyHelperEx(DisplayOptions, m_aabb, "Bounding Box", IProperty::Flags::None);
        _desc.registerPropertyHelperEx(DisplayOptions, m_wireframe,  "Wireframe", IProperty::Flags::SameLine);

        _desc.registerPropertyHelperEx(DisplayOptions, m_opaque, "Opaque", IProperty::Flags::None);
        _desc.registerPropertyHelperEx(DisplayOptions, m_postProcess, "PostProcess", IProperty::Flags::SameLine);     

        _desc.registerPropertyHelper(DisplayOptions, m_raytracing, "Ray Tracing (Experimental)");

        // TODO: Move to menu or toolbar instead
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
            auto value = *_prop->GetPropertyEnum<gfx::VSync>(this);
            Renderer::get()->SetVSync(value);
        }
    }
}