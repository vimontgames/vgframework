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
        m_displayFlags(DisplayFlags::AlbedoMap | DisplayFlags::NormalMap),
        m_renderPassFlags(RenderPassFlags::ZPrepass | RenderPassFlags::Opaque | RenderPassFlags::Transparency | RenderPassFlags::PostProcess)
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

            auto vSyncProp = _object->getClassDesc()->GetPropertyByName("m_VSync");
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

        _desc.registerProperty(DisplayOptions, m_toolMode, "Toolmode");

        _desc.registerProperty(DisplayOptions, m_aabb, "Bounding Box");
        _desc.registerPropertyEx(DisplayOptions, m_wireframe, "Wireframe", IProperty::Flags::SameLine);

        _desc.registerPropertyEnum(DisplayOptions, DisplayMode, m_debugDisplayMode, "Mode");
        _desc.registerPropertyEnumBitfield(DisplayOptions, DisplayFlags, m_displayFlags, "Flags");
        _desc.registerPropertyEnumBitfield(DisplayOptions, RenderPassFlags, m_renderPassFlags, "Render Passes");

        _desc.registerPropertyEx(DisplayOptions, m_backgroundColor, "Background", IProperty::Flags::Color);

        _desc.registerPropertyEnum(DisplayOptions, gfx::VSync, m_VSync, "VSync");

        // TODO: Move to menu or toolbar instead
        _desc.registerPropertyCallback(DisplayOptions, load, "Load");
        _desc.registerPropertyCallbackEx(DisplayOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void DisplayOptions::onPropertyChanged(IObject * _object, const core::IProperty & _prop)
    {
        const char * name = _prop.getName();
        if (!strcmp(name, "m_VSync"))
        {
            ApplyVsync(&_prop);
        }
        else if (!strcmp(name, "m_backgroundColor"))
        {
            const auto backgroundColor = m_backgroundColor;
            m_backgroundColor = (float4)0.0f;
            setBackgroundColor(backgroundColor);
        }
    }

    //--------------------------------------------------------------------------------------
    // Clear color is part of the RenderTarget descriptor, thus we need to reset the pool when changing it
    //--------------------------------------------------------------------------------------
    void DisplayOptions::setBackgroundColor(const core::float4 & _backgroundColor)
    {
        if (any(_backgroundColor != m_backgroundColor))
        {
            auto * renderer = Renderer::get();
            renderer->SetResized();
            m_backgroundColor = _backgroundColor;
        }
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