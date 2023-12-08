#include "DisplayOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"
#include "renderer/Renderer.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(DisplayOptions, "Display Options");

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DisplayOptions, m_toolMode, "Toolmode");
        registerPropertyEx(DisplayOptions, m_wireframe, "Wireframe", IProperty::Flags::SameLine);

        registerProperty(DisplayOptions, m_aabb, "Bounding Box");
        registerPropertyEx(DisplayOptions, m_colliders, "Colliders", IProperty::Flags::SameLine);

        registerPropertyEnum(DisplayOptions, DisplayMode, m_debugDisplayMode, "Mode");
        registerPropertyEnumBitfield(DisplayOptions, DisplayFlags, m_displayFlags, "Flags");
        registerPropertyEnumBitfield(DisplayOptions, RenderPassFlags, m_renderPassFlags, "RenderPass");

        registerPropertyEx(DisplayOptions, m_backgroundColor, "Background", IProperty::Flags::Color);
        registerPropertyEnum(DisplayOptions, GUITheme, m_guiTheme, "Theme");

        registerPropertyEnum(DisplayOptions, gfx::VSync, m_VSync, "VSync");

        // TODO: Move to menu or toolbar instead
        registerPropertyCallback(DisplayOptions, load, "Load");
        registerPropertyCallbackEx(DisplayOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    static const char * filename = "DisplayOptions.xml";

    //--------------------------------------------------------------------------------------
    DisplayOptions::DisplayOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_debugDisplayMode(DisplayMode::Default),
        m_displayFlags(DisplayFlags::AlbedoMap | DisplayFlags::NormalMap),
        m_renderPassFlags(RenderPassFlags::ZPrepass | RenderPassFlags::Opaque | RenderPassFlags::Transparency | RenderPassFlags::PostProcess)
    {
        load(this);
        ImGuiThemesManager::set(m_guiTheme);
    }

    //--------------------------------------------------------------------------------------
    bool DisplayOptions::load(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        if (factory->loadFromXML(_object, filename))
        {
            DisplayOptions * displayOptions = static_cast<DisplayOptions *>(_object);

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
    void DisplayOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
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
        else if (!strcmp(name, "m_guiTheme"))
        {
            ImGuiThemesManager::set(m_guiTheme);
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