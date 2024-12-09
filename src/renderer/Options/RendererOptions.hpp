#include "RendererOptions.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"
#include "core/IResource.h"
#include "gfx/ITexture.h"
#include "gfx/Device/DeviceCaps.h"
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
        setPropertyDescription(RendererOptions, m_toolMode, "Enable Toolmode in Game views");

        //registerPropertyEnumEx(RendererOptions, DisplayMode, m_debugDisplayMode, "Debug", PropertyFlags::AlphabeticalOrder);
        registerPropertyEnum(RendererOptions, DisplayMode, m_debugDisplayMode, "Debug");

        registerPropertyGroupBegin(RendererOptions, "Lighting");
        {
            registerPropertyEnum(RendererOptions, LightingMode, m_lightingMode, "Mode");
            setPropertyDescription(RendererOptions, m_lightingMode, "Lighting monde will affect how lights are computed.\nIn \"Forward\" mode lighting is computed on the fly in pixel shader\nIn \"Defered\" mode lighting is computed in screen-space");

            registerPropertyEnumBitfield(RendererOptions, PBRFlags, m_pbrFlags, "PBR"); 
            setPropertyDescription(RendererOptions, m_pbrFlags, "PBR lighting flags");

            registerPropertyGroupBegin(RendererOptions, "Default environment");
            {
                registerPropertyEx(RendererOptions, m_defaultEnvironmentColor, "Color", PropertyFlags::Color);
                setPropertyDescription(RendererOptions, m_defaultEnvironmentColor, "Default environment color is used as fallback when no environment cubemap is provided");

                // We cannot use 'TextureResource' to reference cubemap file from Renderer, so we need to use a 'ResourcePtr' here and create the 'TextureResource' using the factory
                registerOptionalPropertyResourcePtr(RendererOptions, m_useDefaultEnvironmentCubemap, m_defaultEnvironmentCubemap, "Cubemap");
                setPropertyDescription(RendererOptions, m_defaultEnvironmentCubemap, "Default environment cubemap to use for ambient diffuse and specular lighting");

                registerProperty(RendererOptions, m_defaultIrradianceIntensity, "Irradiance");
                setPropertyRange(RendererOptions, m_defaultIrradianceIntensity, float2(0, 10));
                setPropertyDescription(RendererOptions, m_defaultIrradianceIntensity, "Default irradiance intensity\nAdjusts the intensity of ambient light reaching surfaces from all directions");

                registerProperty(RendererOptions, m_defaultSpecularReflectionIntensity, "Reflection");
                setPropertyRange(RendererOptions, m_defaultSpecularReflectionIntensity, float2(0, 10));
                setPropertyDescription(RendererOptions, m_defaultSpecularReflectionIntensity, "Default specular reflection intensity\nAdjusts the strength of reflections on shiny surfaces");
            }
            registerPropertyGroupEnd(RendererOptions);
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Presentation"); 
        {
            registerPropertyEnum(RendererOptions, gfx::HDR, m_HDRmode, "HDR");
            setPropertyDescription(RendererOptions, m_HDRmode, "High-dynamic range display mode");

            registerPropertyEnum(RendererOptions, gfx::MSAA, m_msaa, "MSAA");
            setPropertyDescription(RendererOptions, m_msaa, "Multisample anti-aliasing");

            registerPropertyEnum(RendererOptions, gfx::VSync, m_VSync, "VSync");
            setPropertyDescription(RendererOptions, m_VSync, "Sync display frequency with monitor refresh rate");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Raytracing");
        {
            registerProperty(RendererOptions, m_rayTracing, "Enable");
            setPropertyDescription(RendererOptions, m_rayTracing, "Enable Raytracing");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Post-process");
        {
            registerProperty(RendererOptions, m_postProcess, "Enable");
            setPropertyDescription(RendererOptions, m_postProcess, "Enable Post-Process");

            registerPropertyEnum(RendererOptions, gfx::AAPostProcess, m_aaPostProcess, "Anti-aliasing");
            setPropertyDescription(RendererOptions, m_aaPostProcess, "Post-Process anti-aliasing");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Framegraph");
        {
            registerPropertyEnumBitfield(RendererOptions, RenderPassFlags, m_renderPassFlags, "Passes");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Materials");
        {
            registerPropertyEnumBitfield(RendererOptions, DisplayFlags, m_displayFlags, "Features");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Misc");
        {
            registerPropertyEx(RendererOptions, m_wireframe, "Wireframe", PropertyFlags::SingleLine);
            setPropertyDescription(RendererOptions, m_wireframe, "Show Wireframe");

            registerPropertyEx(RendererOptions, m_aabb, "Bounding Box", PropertyFlags::SingleLine);
            setPropertyDescription(RendererOptions, m_aabb, "Show Bounding Boxes");

            registerPropertyEx(RendererOptions, m_debugUI, "Debug UI", PropertyFlags::SingleLine);
            setPropertyDescription(RendererOptions, m_debugUI, "Show UI debug");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Device");
        {
            registerPropertyObjectPtrEx(RendererOptions, m_deviceCaps, "Device Caps", PropertyFlags::Flatten | PropertyFlags::NotSaved);
        }
        registerPropertyGroupEnd(RendererOptions);

        return true;
    }

    //--------------------------------------------------------------------------------------
    RendererOptions::RendererOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_renderPassFlags(RenderPassFlags::ZPrepass | RenderPassFlags::Opaque | RenderPassFlags::Transparency)
    {
        SetFile("Renderer.xml");
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::createResources()
    {
        // Must be created before load
        const auto * factory = Kernel::getFactory();
        m_defaultEnvironmentCubemap = (core::IResource *)factory->CreateObject("TextureResource");
        m_defaultEnvironmentCubemap->SetParent(this);

        Load();

        auto * renderer = Renderer::get();
        renderer->SetVSync(m_VSync);
        renderer->SetHDR(m_HDRmode);
        RayTracingManager::get()->enableRayTracing(m_rayTracing);

        auto * classDesc = GetClassDesc();
        m_hdrProp = classDesc->GetPropertyByName("m_HDRmode");
        m_vsyncProp = classDesc->GetPropertyByName("m_VSync");
        m_msaaProp = classDesc->GetPropertyByName("m_msaa");
        m_aaPostProcessProp = classDesc->GetPropertyByName("m_aaPostProcess");

        // Disable incompatible MSAA modes
        for (uint i = 0; i < enumCount<gfx::MSAA>(); ++i)
        {
            const auto msaa = enumValue<gfx::MSAA>(i);
            if (renderer->IsMSAASupported(msaa))
                m_msaaProp->SetEnumValueFlags((u64)msaa, EnumValueFlags::Disabled, false);
            else
                m_msaaProp->SetEnumValueFlags((u64)msaa, EnumValueFlags::Disabled, true);
        }

        // SMAA not yet implemented
        m_aaPostProcessProp->SetEnumValueFlags((u64)gfx::AAPostProcess::SMAA, EnumValueFlags::Disabled, true);

        m_deviceCaps = &renderer->getDeviceCaps();

        // User-friendly names for shader model
        if (auto * shaderModelProp = m_deviceCaps->GetClassDesc()->GetPropertyByName("shaderModel"))
        {
            for (uint i = 0; i < enumCount<gfx::ShaderModel>(); ++i)
            {
                string name = asString(enumValue<gfx::ShaderModel>(i));
                name.replace(0, 3, "");
                name.replace(1, 1, ".");
                shaderModelProp->SetEnumName(i, name);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::releaseResources()
    {
        VG_SAFE_RELEASE(m_defaultEnvironmentCubemap);
    }

    //--------------------------------------------------------------------------------------
    RendererOptions::~RendererOptions()
    {
        
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * RendererOptions::GetDefaultCubemap() const
    {
        if (m_useDefaultEnvironmentCubemap && m_defaultEnvironmentCubemap)
        {
            if (auto * cubemap = VG_SAFE_STATIC_CAST(gfx::ITexture, m_defaultEnvironmentCubemap->GetObject()))
                return cubemap;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.GetName();
        if (!strcmp(name, "m_VSync"))
        {
            applyVSync(&_prop);
        }
        else if (!strcmp(name, "m_HDRmode"))
        {
            applyHDR(&_prop);
        }
        else if (!strcmp(name, "m_msaa"))
        {
            applyMSAA(&_prop);
        }
        else if (!strcmp(name, "m_defaultClearColor"))
        {
            const auto backgroundColor = m_defaultEnvironmentColor;
            m_defaultEnvironmentColor = (float4)0.0f;
            setDefaultClearColor(backgroundColor);
        }
        else if (!strcmp(name, "m_rayTracing"))
        {
            RayTracingManager::get()->enableRayTracing(isRayTracingEnabled());
        }
    }

    //--------------------------------------------------------------------------------------
    // Clear color is part of the RenderTarget descriptor, thus we need to reset the pool when changing it
    //--------------------------------------------------------------------------------------
    void RendererOptions::setDefaultClearColor(const core::float4 & _backgroundColor)
    {
        if (any(_backgroundColor != m_defaultEnvironmentColor))
        {
            auto * renderer = Renderer::get();
            renderer->SetResized();
            m_defaultEnvironmentColor = _backgroundColor;
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::VSync RendererOptions::GetVSync() const
    {
        return m_VSync;
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::SetVSync(gfx::VSync _vsync)
    {
        if (m_VSync != _vsync)
        {
            m_VSync = _vsync;
            applyVSync(m_vsyncProp);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    gfx::MSAA RendererOptions::GetMSAA() const
    {
        return m_msaa;
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::SetMSAA(gfx::MSAA _msaa)
    {
        if (m_msaa != _msaa)
        {
            m_msaa = _msaa;
            applyMSAA(m_msaaProp);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    gfx::AAPostProcess RendererOptions::GetAAPostProcess() const
    {
        return m_aaPostProcess;
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::SetAAPostProcess(gfx::AAPostProcess _aa)
    {
        if (m_aaPostProcess != _aa)
        {
            m_aaPostProcess = _aa;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    gfx::HDR RendererOptions::GetHDR() const
    {
        return m_HDRmode;
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::SetHDR(gfx::HDR _hdr)
    {
        if (m_HDRmode != _hdr)
        {
            m_HDRmode = _hdr;
            applyHDR(m_hdrProp);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::applyVSync(const core::IProperty * _prop)
    {
        if (nullptr != _prop)
        {
            auto value = *_prop->GetPropertyEnum<gfx::VSync>(this);
            Renderer::get()->SetVSync(value);
        }
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::applyHDR(const core::IProperty * _prop)
    {
        if (nullptr != _prop)
        {
            auto value = *_prop->GetPropertyEnum<gfx::HDR>(this);
            Renderer::get()->SetHDR(value);
        }
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::applyMSAA(const core::IProperty * _prop)
    {
        if (nullptr != _prop)
        {
            auto value = *_prop->GetPropertyEnum<gfx::MSAA>(this);
            //Renderer::get()->SetMSAA(value);
        }
    }    

    //--------------------------------------------------------------------------------------
    bool RendererOptions::anyRayTracingDebugDisplay() const
    {
        return asInteger(m_debugDisplayMode) >= asInteger(DisplayMode::RayTracing_Committed_Hit) && asInteger(m_debugDisplayMode) < asInteger(DisplayMode::PostProcess_Depth);
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
                auto mode = enumValue<gfx::HDR>(i);
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
                auto mode = enumValue<gfx::VSync>(i);
                bool supported = renderer->IsVSyncSupported(mode);

                if (m_vsyncProp->SetEnumValueFlags(m_vsyncProp->GetUnsignedEnumValue(i), EnumValueFlags::Disabled, !supported))
                {
                    // changed
                }
            }
        }
    }
}