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

        registerPropertyEnum(RendererOptions, gfx::VSync, m_VSync, "VSync");
        registerPropertyEx(RendererOptions, m_backgroundColor, "Background", IProperty::Flags::Color);
        registerPropertyEnumBitfield(RendererOptions, RenderPassFlags, m_renderPassFlags, "Passes");

        registerPropertyGroupBegin(RendererOptions, "Materials");
        {
            registerPropertyEnum(RendererOptions, DisplayMode, m_debugDisplayMode, "Mode");
            registerPropertyEnumBitfield(RendererOptions, DisplayFlags, m_displayFlags, "Flags");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "RayTracing");
        {
            registerProperty(RendererOptions, m_rayTracing, "Enable");
            registerPropertyEnum(RendererOptions, RayTracingMode, m_rayTracingMode, "Mode");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "PostProcess");
        {
            registerProperty(RendererOptions, m_postProcess, "Enable");
            registerPropertyEnum(RendererOptions, PostProcessMode, m_postProcessMode, "Mode");
        }
        registerPropertyGroupEnd(RendererOptions);

        return true;
    }

    //--------------------------------------------------------------------------------------
    RendererOptions::RendererOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_renderPassFlags(RenderPassFlags::ZPrepass | RenderPassFlags::Opaque | RenderPassFlags::Transparency)
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
        else if (!strcmp(name, "m_rayTracing"))
        {
            RayTracingManager::get()->enableRayTracing(isRayTracingEnabled());
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