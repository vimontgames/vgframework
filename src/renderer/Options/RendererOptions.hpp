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

        registerPropertyEx(RendererOptions, m_toolMode, "Toolmode", PropertyFlags::SingleLine);
        setPropertyDescription(LightDesc, m_toolMode, "Enable Toolmode in Game views");

        registerPropertyEx(RendererOptions, m_rayTracing, "RayTracing", PropertyFlags::SingleLine);
        setPropertyDescription(LightDesc, m_rayTracing, "Enable Ray-Tracing features");

        registerPropertyEx(RendererOptions, m_postProcess, "PostProcess", PropertyFlags::SingleLine);
        setPropertyDescription(RendererOptions, m_postProcess, "Enable Post-Process features");

        registerPropertyEnum(RendererOptions, LightingMode, m_lightingMode, "Lighting");
        setPropertyDescription(LightDesc, m_lightingMode, "Lighting monde will affect how lights are computed.\nIn \"Forward\" mode lighting is computed on the fly in pixel shader\nIn \"Defered\" mode lighting is computed in screen-space");

        registerPropertyEnum(RendererOptions, DisplayMode, m_debugDisplayMode, "Debug");
        registerPropertyEnumBitfield(RendererOptions, DisplayFlags, m_displayFlags, "Flags");

        registerPropertyGroupBegin(RendererOptions, "Advanced");
        {
            registerPropertyEx(RendererOptions, m_wireframe, "Wireframe", PropertyFlags::SingleLine);
            setPropertyDescription(LightDesc, m_wireframe, "Show Wireframe");

            registerPropertyEx(RendererOptions, m_aabb, "Bounding Box", PropertyFlags::SingleLine);
            setPropertyDescription(LightDesc, m_aabb, "Show Bounding Boxes");

            registerPropertyEx(RendererOptions, m_debugUI, "Debug UI", PropertyFlags::SingleLine);
            setPropertyDescription(LightDesc, m_debugUI, "Show UI debug");

            registerPropertyEnum(RendererOptions, gfx::VSync, m_VSync, "VSync");
            setPropertyDescription(LightDesc, m_VSync, "Sync display frequency with monitor refresh rate");

            registerPropertyEnum(RendererOptions, gfx::HDR, m_HDRmode, "HDR");
            setPropertyDescription(LightDesc, m_HDRmode, "High-dynamic range display mode");

            registerPropertyEx(RendererOptions, m_backgroundColor, "Background", PropertyFlags::Color);
            setPropertyDescription(LightDesc, m_backgroundColor, "Scene background color");

            registerPropertyEnumBitfield(RendererOptions, RenderPassFlags, m_renderPassFlags, "Passes");
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

        auto * renderer = Renderer::get();
        renderer->SetVSync(m_VSync);
        renderer->SetHDR(m_HDRmode);
        RayTracingManager::get()->enableRayTracing(m_rayTracing);

        m_hdrProp = GetClassDesc()->GetPropertyByName("m_HDRmode");
        m_vsyncProp = GetClassDesc()->GetPropertyByName("m_VSync"); 
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.GetName();
        if (!strcmp(name, "m_VSync"))
        {
            ApplyVsync(&_prop);
        }
        if (!strcmp(name, "m_HDRmode"))
        {
            ApplyHDR(&_prop);
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

    //--------------------------------------------------------------------------------------
    void RendererOptions::ApplyHDR(const core::IProperty * _prop)
    {
        if (nullptr != _prop)
        {
            auto value = *_prop->GetPropertyEnum<gfx::HDR>(this);
            Renderer::get()->SetHDR(value);
        }
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::anyRayTracingDebugDisplay() const
    {
        return asInteger(m_debugDisplayMode) >= asInteger(DisplayMode::RayTracing_Hit) && asInteger(m_debugDisplayMode) < asInteger(DisplayMode::PostProcess_Depth);
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::update() const
    {
        auto * renderer = Renderer::get();

        if (m_hdrProp)
        {
            // Update HDR
            for (uint i = 0; i < enumCount<gfx::HDR>(); ++i)
            {
                auto mode = (gfx::HDR)i;
                bool supported = renderer->IsHDRSupported(mode);

                if (m_hdrProp->SetEnumValueFlags(m_hdrProp->GetUnsignedEnumValue(i), EnumValueFlags::Disabled, !supported))
                {
                    // changed
                }
            }
        }

        if (m_vsyncProp)
        {
            for (uint i = 0; i < enumCount<gfx::VSync>(); ++i)
            {
                auto mode = (gfx::VSync)i;
                bool supported = renderer->IsVSyncSupported(mode);

                if (m_vsyncProp->SetEnumValueFlags(m_vsyncProp->GetUnsignedEnumValue(i), EnumValueFlags::Disabled, !supported))
                {
                    // changed
                }
            }
        }
    }
}