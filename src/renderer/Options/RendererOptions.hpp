#include "RendererOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"
#include "renderer/Renderer.h"
#include "renderer/RayTracing/RayTracingManager.h"

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(RendererOptions, "Renderer Options");

    //--------------------------------------------------------------------------------------
    bool RendererOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(RendererOptions, m_toolMode, "Toolmode");

        registerProperty(RendererOptions, m_wireframe, "Wireframe");
        registerPropertyEx(RendererOptions, m_aabb, "Bounding Box", IProperty::Flags::SameLine);

        registerPropertyEnum(RendererOptions, DisplayMode, m_debugDisplayMode, "Mode");
        registerPropertyEnumBitfield(RendererOptions, DisplayFlags, m_displayFlags, "Flags");
        registerPropertyEnumBitfield(RendererOptions, RenderPassFlags, m_renderPassFlags, "Passes");

        registerPropertyEx(RendererOptions, m_backgroundColor, "Background", IProperty::Flags::Color);

        registerPropertyEnum(RendererOptions, gfx::VSync, m_VSync, "VSync");

        return true;
    }

    //--------------------------------------------------------------------------------------
    RendererOptions::RendererOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_debugDisplayMode(DisplayMode::Default),
        m_displayFlags(DisplayFlags::AlbedoMap | DisplayFlags::NormalMap),
        m_renderPassFlags(RenderPassFlags::ZPrepass | RenderPassFlags::Opaque | RenderPassFlags::Transparency | RenderPassFlags::PostProcess)
    {
        setFile("Renderer.xml");
        Load();
        Renderer::get()->SetVSync(m_VSync);
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
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
        else if (!strcmp(name, "m_renderPassFlags"))
        {
            RayTracingManager::get()->enableRayTracing(asBool(RenderPassFlags::RayTracing & m_renderPassFlags));
        }
    }

    //--------------------------------------------------------------------------------------
    // Clear color is part of the RenderTarget descriptor, thus we need to reset the pool when changing it
    //--------------------------------------------------------------------------------------
    void RendererOptions::setBackgroundColor(const core::float4 & _backgroundColor)
    {
        if (any(_backgroundColor != m_backgroundColor))
        {
            auto * renderer = Renderer::get();
            renderer->SetResized();
            m_backgroundColor = _backgroundColor;
        }
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::ApplyVsync(const core::IProperty * _prop)
    {
        if (nullptr != _prop)
        {
            auto value = *_prop->GetPropertyEnum<gfx::VSync>(this);
            Renderer::get()->SetVSync(value);
        }
    }
}