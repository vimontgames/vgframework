#include "RendererOptions.h"

#if !VG_ENABLE_INLINE
#include "RendererOptions.inl"
#endif

#include "core/Object/AutoRegisterClass.h"
#include "core/Object/EnumHelper.h"
#include "core/string/string.h"
#include "core/IResource.h"
#include "gfx/ITexture.h"
#include "gfx/Device/Device.h"
#include "gfx/Device/DeviceCaps.h"
#include "renderer/Renderer.h"
#include "renderer/RayTracing/RayTracingManager.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(RendererOptions, "Renderer Options");

    #define setPropertyHiddenCallbackQualityEx(className, propertyName, func)                   setPropertyHiddenCallback(className, propertyName[Quality::VeryLow],  func##VeryLow);   \
                                                                                                setPropertyHiddenCallback(className, propertyName[Quality::Low],      func##Low);       \
                                                                                                setPropertyHiddenCallback(className, propertyName[Quality::Medium],   func##Medium);    \
                                                                                                setPropertyHiddenCallback(className, propertyName[Quality::High],     func##High);      \
                                                                                                setPropertyHiddenCallback(className, propertyName[Quality::VeryHigh], func##VeryHigh);  

    #define setPropertyReadOnlyCallbackQuality(className, propertyName, func)                   setPropertyReadOnlyCallback(className, propertyName[Quality::VeryLow],  func);  \
                                                                                                setPropertyReadOnlyCallback(className, propertyName[Quality::Low],      func);  \
                                                                                                setPropertyReadOnlyCallback(className, propertyName[Quality::Medium],   func);  \
                                                                                                setPropertyReadOnlyCallback(className, propertyName[Quality::High],     func);  \
                                                                                                setPropertyReadOnlyCallback(className, propertyName[Quality::VeryHigh], func);  

    #define registerPropertyQuality(className, propertyName, displayName, func)                 registerProperty(className, propertyName[Quality::VeryLow], displayName);                               \
                                                                                                registerProperty(className, propertyName[Quality::Low],     displayName);                               \
                                                                                                registerProperty(className, propertyName[Quality::Medium],  displayName);                               \
                                                                                                registerProperty(className, propertyName[Quality::High],    displayName);                               \
                                                                                                registerProperty(className, propertyName[Quality::VeryHigh],displayName);                               \
                                                                                                setPropertyHiddenCallbackQualityEx(className, propertyName, func);

    #define registerPropertyEnumQuality(className, enumName, propertyName, displayName, func)   registerPropertyEnum(className, enumName, propertyName[Quality::VeryLow], displayName);            \
                                                                                                registerPropertyEnum(className, enumName, propertyName[Quality::Low],     displayName);            \
                                                                                                registerPropertyEnum(className, enumName, propertyName[Quality::Medium],  displayName);            \
                                                                                                registerPropertyEnum(className, enumName, propertyName[Quality::High],    displayName);            \
                                                                                                registerPropertyEnum(className, enumName, propertyName[Quality::VeryHigh],displayName);            \
                                                                                                setPropertyHiddenCallbackQualityEx(className, propertyName, func);
                                                                                 
    #define setPropertyDescriptionQuality(className, propertyName, desc)                        setPropertyDescription(className, propertyName[Quality::VeryLow], desc " in 'VeryLow' quality mode");  \
                                                                                                setPropertyDescription(className, propertyName[Quality::Low],     desc " in 'Low' quality mode");      \
                                                                                                setPropertyDescription(className, propertyName[Quality::Medium],  desc " in 'Medium' quality mode");   \
                                                                                                setPropertyDescription(className, propertyName[Quality::High],    desc " in 'High' quality mode");     \
                                                                                                setPropertyDescription(className, propertyName[Quality::VeryHigh],desc " in 'VeryHigh' quality mode");

    //--------------------------------------------------------------------------------------
    bool isQualityPropertyHidden(const IObject * _object, const IProperty * _prop, Quality _quality)
    {
        const RendererOptions * rendererOptions = VG_SAFE_STATIC_CAST(const RendererOptions, _object);
        return rendererOptions->getCurrentQualityLevel() != _quality;
    }

    //--------------------------------------------------------------------------------------
    bool isQualityPropertyHiddenVeryLow(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        return isQualityPropertyHidden(_object, _prop, Quality::VeryLow);
    }

    //--------------------------------------------------------------------------------------
    bool isQualityPropertyHiddenLow(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        return isQualityPropertyHidden(_object, _prop, Quality::Low);
    }

    //--------------------------------------------------------------------------------------
    bool isQualityPropertyHiddenMedium(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        return isQualityPropertyHidden(_object, _prop, Quality::Medium);
    }

    //--------------------------------------------------------------------------------------
    bool isQualityPropertyHiddenHigh(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        return isQualityPropertyHidden(_object, _prop, Quality::High);
    }

    //--------------------------------------------------------------------------------------
    bool isQualityPropertyHiddenVeryHigh(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        return isQualityPropertyHidden(_object, _prop, Quality::VeryHigh);
    }

    //--------------------------------------------------------------------------------------
    bool isRenderJobOptionReadOnly(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const RendererOptions * options = VG_SAFE_STATIC_CAST(const RendererOptions, _object);
        return !options->isRenderJobsEnabled();
    }

    //--------------------------------------------------------------------------------------
    bool isRenderJobsCountReadOnly(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const RendererOptions * options = VG_SAFE_STATIC_CAST(const RendererOptions, _object);
        return !options->isRenderJobsEnabled() || !options->isForcedRenderJobsCount();
    }    

    //--------------------------------------------------------------------------------------
    bool isPostProcessPropertyReadOnly(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const RendererOptions * options = VG_SAFE_STATIC_CAST(const RendererOptions, _object);
        return !options->isPostProcessEnabled();
    }    

    //--------------------------------------------------------------------------------------
    bool isShadowPropertyReadOnly(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const RendererOptions * options = VG_SAFE_STATIC_CAST(const RendererOptions, _object);
        return !options->IsShadowEnabled();
    }

    //--------------------------------------------------------------------------------------
    bool IsRaytracingDisabled(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const gfx::DeviceCaps & caps = gfx::Device::get()->getDeviceCaps();
        return caps.rayTracing.supported == false;
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(RendererOptions, m_toolMode, "Toolmode");
        setPropertyDescription(RendererOptions, m_toolMode, "Enable Toolmode in Game views");

        //registerPropertyEnumEx(RendererOptions, DisplayMode, m_debugDisplayMode, "Debug", PropertyFlags::AlphabeticalOrder);
        registerPropertyEnum(RendererOptions, DisplayMode, m_debugDisplayMode, "Debug");

        registerOptionalPropertyEnum(RendererOptions, m_useCustomQualityLevel, Quality, m_customQualityLevel, "Quality");
        setPropertyDescription(RendererOptions, m_customQualityLevel, "Quality level for rendering. Leave unchecked for automatic quality level based on your GPU.");

        registerPropertyGroupBegin(RendererOptions, "Device");
        {
            registerPropertyEnum(RendererOptions, gfx::HDR, m_HDRmode, "HDR");
            setPropertyDescription(RendererOptions, m_HDRmode, "High-dynamic range display mode");

            registerPropertyEnumQuality(RendererOptions, gfx::MSAA, m_msaa, "MSAA", isQualityPropertyHidden);
            setPropertyDescriptionQuality(RendererOptions, m_msaa, "MSAA sample count for antialiasing");

            registerPropertyEnum(RendererOptions, gfx::VSync, m_VSync, "VSync");
            setPropertyDescription(RendererOptions, m_VSync, "Sync display frequency with monitor refresh rate");

            registerPropertyGroupBegin(RendererOptions, "Caps");
            {
                registerPropertyObjectPtrEx(RendererOptions, m_deviceCaps, "Caps", PropertyFlags::Flatten | PropertyFlags::Transient);
            }
            registerPropertyGroupEnd(RendererOptions);
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Lighting");
        {
            registerPropertyEnum(RendererOptions, LightingMode, m_lightingMode, "Mode");
            setPropertyDescription(RendererOptions, m_lightingMode, "Lighting monde will affect how lights are computed.\nIn \"Forward\" mode lighting is computed on the fly in pixel shader\nIn \"Defered\" mode lighting is computed in screen-space");

            registerPropertyGroupBegin(RendererOptions, "Environment");
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

            registerPropertyGroupBegin(RendererOptions, "PBR");
            {
                registerPropertyEnumBitfield(RendererOptions, PBRFlags, m_pbrFlags, "Flags");
                setPropertyDescription(RendererOptions, m_pbrFlags, "Physically-based rendering flags");

                registerPropertyResourcePtr(RendererOptions, m_pbrBakedBRDFTexture, "Baked BRDF");
                setPropertyDescription(RendererOptions, m_pbrBakedBRDFTexture, "Baked specular BRDF texture used when \"Generate specular BRDF\" is not selected.")
            }
            registerPropertyGroupEnd(RendererOptions);

            registerPropertyGroupBegin(RendererOptions, "Raytracing");
            {
                registerProperty(RendererOptions, m_rayTracing, "Enable");
                setPropertyDescription(RendererOptions, m_rayTracing, "Enable Raytracing");
                setPropertyReadOnlyCallback(RendererOptions, m_rayTracing, IsRaytracingDisabled);

                registerPropertyEnum(RendererOptions, TLASMode, m_rayTracingTLASMode, "TLAS");
                setPropertyDescription(RendererOptions, m_rayTracingTLASMode, "Defines how the raytracing Top-Level Accelleration Structure is updated.");
                setPropertyReadOnlyCallback(RendererOptions, m_rayTracingTLASMode, IsRaytracingDisabled);
            }
            registerPropertyGroupEnd(RendererOptions);

            registerPropertyGroupBegin(RendererOptions, "Shadows");
            {
                registerPropertyQuality(RendererOptions, m_shadows, "Cast shadows", isQualityPropertyHidden);
                setPropertyDescriptionQuality(RendererOptions, m_shadows, "Enable realtime shadows");

                registerPropertyEnumQuality(RendererOptions, ShadowDefaultResolution, m_shadowsResolution, "Resolution", isQualityPropertyHidden);
                setPropertyDescriptionQuality(RendererOptions, m_shadowsResolution, "Default resolution for realtime shadows");
                setPropertyReadOnlyCallbackQuality(RendererOptions, m_shadowsResolution, isShadowPropertyReadOnly);
            }
            registerPropertyGroupEnd(RendererOptions);
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Post-process");
        {
            registerProperty(RendererOptions, m_postProcess, "Enable");
            setPropertyDescription(RendererOptions, m_postProcess, "Enable Post-Process");

            registerPropertyEnum(RendererOptions, gfx::AAPostProcess, m_aaPostProcess, "Anti-aliasing");
            setPropertyDescription(RendererOptions, m_aaPostProcess, "Post-Process anti-aliasing");
            setPropertyReadOnlyCallback(RendererOptions, m_aaPostProcess, isPostProcessPropertyReadOnly);

            registerPropertyEnumQuality(RendererOptions, DepthOfFieldMode, m_depthOfField, "Depth of field", isQualityPropertyHidden);
            setPropertyDescriptionQuality(RendererOptions, m_depthOfField, "Select depth of field postprocess quality");
            setPropertyReadOnlyCallbackQuality(RendererOptions, m_depthOfField, isPostProcessPropertyReadOnly);
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Debug");
        {
            registerProperty(RendererOptions, m_aabb, "AABB");
            setPropertyDescription(RendererOptions, m_aabb, "Show Bounding Boxes");

            registerProperty(RendererOptions, m_particles, "Particles");
            setPropertyDescription(RendererOptions, m_particles, "Show particles");

            registerProperty(RendererOptions, m_wireframe, "Wireframe");
            setPropertyDescription(RendererOptions, m_wireframe, "Show wireframe");

            registerProperty(RendererOptions, m_debugUI, "UI");
            setPropertyDescription(RendererOptions, m_debugUI, "Show UI debug");

            registerProperty(RendererOptions, m_debugFrustum, "Frustum");
            setPropertyDescription(RendererOptions, m_debugFrustum, "Show camera frustum");

            registerPropertyEnumBitfield(RendererOptions, DisplayFlags, m_displayFlags, "Features");
            registerPropertyEnumBitfield(RendererOptions, RenderPassFlags, m_renderPassFlags, "Passes");
        }
        registerPropertyGroupEnd(RendererOptions);

        registerPropertyGroupBegin(RendererOptions, "Jobs");
        {
            registerPropertyGroupBegin(RendererOptions, "Culling");
            {
                registerProperty(RendererOptions, m_splitCullingViewJobs, "Split Culling");
                setPropertyDescription(RendererOptions, m_splitCullingViewJobs, "Use several culling jobs per view");
            }
            registerPropertyGroupEnd(RendererOptions);

            registerPropertyGroupBegin(RendererOptions, "Render");
            {
                registerProperty(RendererOptions, m_renderJobs, "Enable");
                setPropertyDescription(RendererOptions, m_renderJobs, "Enable render jobs");

                registerProperty(RendererOptions, m_forceRenderJobsCount, "Force job count");
                setPropertyDescription(RendererOptions, m_forceRenderJobsCount, "Override RenderJobs count");
                setPropertyReadOnlyCallback(RendererOptions, m_forceRenderJobsCount, isRenderJobOptionReadOnly);

                registerProperty(RendererOptions, m_renderJobsCount, "Job count");
                setPropertyDescription(RendererOptions, m_renderJobsCount, "Forced job count");
                setPropertyRange(RendererOptions, m_renderJobsCount, float2(2, 64));
                setPropertyHiddenCallback(RendererOptions, m_renderJobsCount, isRenderJobsCountReadOnly);

                registerPropertyEnum(RendererOptions, gfx::RenderJobsPolicy, m_renderJobsPolicy, "Policy");
                setPropertyDescription(RendererOptions, m_renderJobsPolicy, "RenderJobs scheduling policy.");
                setPropertyReadOnlyCallback(RendererOptions, m_renderJobsPolicy, isRenderJobOptionReadOnly);

                registerProperty(RendererOptions, m_renderJobsTotalBufferSizeInMB, "Total size");
                setPropertyDescription(RendererOptions, m_renderJobsTotalBufferSizeInMB, "Total size of the upload buffers used by render jobs in MB.");
                setPropertyReadOnlyCallback(RendererOptions, m_renderJobsTotalBufferSizeInMB, isRenderJobOptionReadOnly);

                registerProperty(RendererOptions, m_renderJobsWorkerMinBufferSizeInMB, "Min size");
                setPropertyDescription(RendererOptions, m_renderJobsWorkerMinBufferSizeInMB, "Minimum size of the upload buffer used by one render jobs in MB.");
                setPropertyReadOnlyCallback(RendererOptions, m_renderJobsWorkerMinBufferSizeInMB, isRenderJobOptionReadOnly);
            }
            registerPropertyGroupEnd(RendererOptions);
        }
        registerPropertyGroupEnd(RendererOptions);

        return true;
    }

    //--------------------------------------------------------------------------------------
    RendererOptions::RendererOptions(const core::string & _name, core::IObject * _parent, bool _safeCopy) :
        super(_name, _parent),
        m_renderPassFlags(RenderPassFlags::ZPrepass | RenderPassFlags::Opaque | RenderPassFlags::Transparency)
    {
        if (_safeCopy)
        {
            s_rendererOptionsSafeCopy = this;
        }
        else
        {
            SetFile("Renderer.xml");

            for (uint i = 0; i < enumCount<Quality>(); ++i)
            {
                m_msaa[i] = gfx::MSAA::None;
                m_shadows[i] = true;
                m_shadowsResolution[i] = ShadowDefaultResolution::ShadowDefaultResolution_1024;
                m_depthOfField[i] = DepthOfFieldMode::Default;
            }

            // Default shadow quality
            m_shadowsResolution[Quality::Low] = ShadowDefaultResolution::ShadowDefaultResolution_512;
            m_shadows[Quality::Low] = false; // Disable shadows for the less detailed quality level

            m_shadowsResolution[Quality::Low] = ShadowDefaultResolution::ShadowDefaultResolution_1024;
            m_shadowsResolution[Quality::Medium] = ShadowDefaultResolution::ShadowDefaultResolution_2048;
            m_shadowsResolution[Quality::High] = ShadowDefaultResolution::ShadowDefaultResolution_4096;
            m_shadowsResolution[Quality::VeryHigh] = ShadowDefaultResolution::ShadowDefaultResolution_4096; // Will ultimately use RT anyway

            // Disable DOF for lower quality level
            m_depthOfField[Quality::Low] = DepthOfFieldMode::None;

            m_customQualityLevel = autodetectQualityLevel();
        }
    }

    static bool g_showRaytracingNotSupportedWarningOnce = true;

    //--------------------------------------------------------------------------------------
    void RendererOptions::sync(const RendererOptions & _other)
    {
        m_toolMode = _other.m_toolMode;
        m_rayTracingTLASMode = _other.m_rayTracingTLASMode;
        m_useCustomQualityLevel = _other.m_useCustomQualityLevel;
        m_customQualityLevel = _other.m_customQualityLevel;
        m_autodetectedQualityLevel = _other.m_autodetectedQualityLevel;
        m_previousQualityLevel = _other.m_previousQualityLevel;
        m_defaultEnvironmentColor = _other.m_defaultEnvironmentColor;
        m_useDefaultEnvironmentCubemap = _other.m_useDefaultEnvironmentCubemap;
        m_defaultEnvironmentCubemap = _other.m_defaultEnvironmentCubemap;
        m_defaultIrradianceIntensity = _other.m_defaultIrradianceIntensity;
        m_defaultSpecularReflectionIntensity = _other.m_defaultSpecularReflectionIntensity;
        m_aabb = _other.m_aabb;
        m_wireframe = _other.m_wireframe;
        m_debugUI = _other.m_debugUI;
        m_debugFrustum = _other.m_debugFrustum;
        m_splitCullingViewJobs = _other.m_splitCullingViewJobs;
        m_particles = _other.m_particles;
        m_postProcess = _other.m_postProcess;
        m_HDRmode = _other.m_HDRmode;
        m_aaPostProcess = _other.m_aaPostProcess;
        m_VSync = _other.m_VSync;
        m_lightingMode = _other.m_lightingMode;
        m_pbrFlags = _other.m_pbrFlags;
        m_pbrBakedBRDFTexture = _other.m_pbrBakedBRDFTexture;
        m_debugDisplayMode = _other.m_debugDisplayMode;
        m_displayFlags = _other.m_displayFlags;
        m_renderPassFlags = _other.m_renderPassFlags;
        m_renderJobs = _other.m_renderJobs;
        m_forceRenderJobsCount = _other.m_forceRenderJobsCount;
        m_renderJobsCount = _other.m_renderJobsCount;
        m_renderJobsPolicy = _other.m_renderJobsPolicy;
        m_renderJobsTotalBufferSizeInMB = _other.m_renderJobsTotalBufferSizeInMB;
        m_renderJobsWorkerMinBufferSizeInMB = _other.m_renderJobsWorkerMinBufferSizeInMB;
        m_deviceCaps = _other.m_deviceCaps;

        for (uint i = 0; i < core::enumCount<Quality>(); ++i)
        {
            m_depthOfField[i] = _other.m_depthOfField[i];
            m_msaa[i] = _other.m_msaa[i];
            m_shadows[i] = _other.m_shadows[i];
            m_shadowsResolution[i] = _other.m_shadowsResolution[i];
        }

        // Enable raytracing only if supported
        const gfx::DeviceCaps & caps = gfx::Device::get()->getDeviceCaps();
        bool raytracing = _other.m_rayTracing;

        // Display warning only once when changes        
        if (raytracing && !caps.rayTracing.supported)
        {
            if (g_showRaytracingNotSupportedWarningOnce)
            {
                VG_ERROR("[Renderer] Raytracing is not supported");
                g_showRaytracingNotSupportedWarningOnce = false;
            }
            raytracing = false;
        }
        if (raytracing != m_rayTracing)
        {
            if (_other.m_rayTracing)
            {
                m_rayTracing = true;
                RayTracingManager::get()->onEnableRayTracing();
            }
            else
            {
                m_rayTracing = false;
                RayTracingManager::get()->onDisableRayTracing();
            }
        }
    }

    //--------------------------------------------------------------------------------------
    Quality RendererOptions::autodetectQualityLevel()
    {
        auto * renderer = Renderer::get();
        const gfx::DeviceCaps & caps = renderer->getDeviceCaps();

        // caps already store memory in MB
        if (caps.memory.dedicated <= 4 * 1024)
            m_autodetectedQualityLevel = Quality::Low;
        else if (caps.memory.dedicated >= 10 * 1024) 
            m_autodetectedQualityLevel = Quality::High;
        else
            m_autodetectedQualityLevel = Quality::Medium;

        return m_autodetectedQualityLevel;
    }

    //--------------------------------------------------------------------------------------
    Quality RendererOptions::getCurrentQualityLevel() const
    {
        return m_useCustomQualityLevel ? m_customQualityLevel : m_autodetectedQualityLevel;
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::createResources()
    {
        // Must be created before load
        const auto * factory = Kernel::getFactory();
        m_defaultEnvironmentCubemap = (core::IResource *)factory->CreateObject("TextureResource");
        m_defaultEnvironmentCubemap->SetParent(this);
        m_defaultEnvironmentCubemap->RegisterUID();

        m_pbrBakedBRDFTexture = (core::IResource *)factory->CreateObject("TextureResource");
        m_pbrBakedBRDFTexture->SetParent(this);
        m_pbrBakedBRDFTexture->RegisterUID();        
        //m_pbrBakedBRDFTexture->SetResourcePath("data/Engine/BRDF/CookTorrance.png"); // can't default value from because loading will load it again and it will leak

        Load(false);

        if (m_pbrBakedBRDFTexture->GetResourcePath().empty())
            m_pbrBakedBRDFTexture->SetResourcePath("data/Engine/BRDF/CookTorrance.png");

        if (!m_useCustomQualityLevel)
            m_customQualityLevel = autodetectQualityLevel();

        auto * renderer = Renderer::get();
        renderer->SetVSync(m_VSync);
        renderer->SetHDR(m_HDRmode);

        auto * classDesc = GetClassDesc();
        m_hdrProp = classDesc->GetPropertyByName("m_HDRmode");
        m_vsyncProp = classDesc->GetPropertyByName("m_VSync");
        m_aaPostProcessProp = classDesc->GetPropertyByName("m_aaPostProcess");

        // Disable incompatible MSAA modes
        for (uint q = 0; q < enumCount<Quality>(); ++q)
        {
            auto & prop = m_msaaProp[q];
            auto propName = fmt::sprintf("m_msaa[Quality::%s]", asString((Quality)q));
            prop = classDesc->GetPropertyByName(propName.c_str());
            VG_ASSERT(prop, "Could not find property \"%s\"", propName.c_str());
            if (nullptr != prop)
            {
                for (uint i = 0; i < enumCount<gfx::MSAA>(); ++i)
                {
                    const auto msaa = enumValue<gfx::MSAA>(i);
                    if (renderer->IsMSAASupported(msaa))
                        prop->SetEnumValueFlags((u64)msaa, EnumValueFlags::Disabled, false);
                    else
                        prop->SetEnumValueFlags((u64)msaa, EnumValueFlags::Disabled, true);
                }
            }
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
        VG_SAFE_RELEASE(m_pbrBakedBRDFTexture);
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
    gfx::ITexture * RendererOptions::GetBakedSpecularBRDF() const
    {
        if (!asBool(PBRFlags::GenerateSpecularBRDF & getPBRFlags()))
        {
            if (auto * brdf = VG_SAFE_STATIC_CAST(gfx::ITexture, m_pbrBakedBRDFTexture->GetObject()))
                return brdf;
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
        else if (strstr(name, "m_msaa"))
        {
            applyMSAA(&_prop);
        }
        else if (!strcmp(name, "m_defaultClearColor"))
        {
            const auto backgroundColor = m_defaultEnvironmentColor;
            m_defaultEnvironmentColor = (float4)0.0f;
            setDefaultClearColor(backgroundColor);
        }
        else if (!strcmp(name, "m_useCustomQualityLevel"))
        {
            applyQualityLevel(&_prop);
        }
        else if (!strcmp(name, "m_rayTracing"))
        {
            g_showRaytracingNotSupportedWarningOnce = true;
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
        return m_msaa[getCurrentQualityLevel()];
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::SetMSAA(gfx::MSAA _msaa)
    {
        auto & msaa = m_msaa[getCurrentQualityLevel()];
        if (msaa != _msaa)
        {
            msaa = _msaa;
            applyMSAA(m_msaaProp[getCurrentQualityLevel()]);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::IsShadowEnabled() const
    {
        return m_shadows[getCurrentQualityLevel()];
    }

    //--------------------------------------------------------------------------------------
    uint2 RendererOptions::GetShadowDefaultResolution() const
    {
        return m_shadowsResolution[getCurrentQualityLevel()];
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
    DepthOfFieldMode RendererOptions::GetDepthOfFieldMode() const
    {
        return m_depthOfField[getCurrentQualityLevel()];
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::SetDepthOfFieldMode(DepthOfFieldMode _depthOfField)
    {
        auto & current = m_depthOfField[getCurrentQualityLevel()];

        if (current != _depthOfField)
        {
            current = _depthOfField;
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
    void RendererOptions::applyQualityLevel(const core::IProperty * _prop)
    {
        m_autodetectedQualityLevel = autodetectQualityLevel();

        if (!m_useCustomQualityLevel)
        {
            m_previousQualityLevel = m_customQualityLevel;
            m_customQualityLevel = m_autodetectedQualityLevel;
        }
        else
        {
            if ((Quality)-1 != m_previousQualityLevel)
                m_customQualityLevel = m_previousQualityLevel;
        }
    }

    //--------------------------------------------------------------------------------------
    bool RendererOptions::anyRayTracingDebugDisplay() const
    {
        return asInteger(m_debugDisplayMode) >= asInteger(DisplayMode::RayTracing_Instance_WorldPosition) && asInteger(m_debugDisplayMode) <= asInteger(DisplayMode::RayTracing_Material_Albedo);
    }

    //--------------------------------------------------------------------------------------
    void RendererOptions::Update()
    {
        super::Update();

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

        RayTracingManager::get()->beginFrame();
    }

    //--------------------------------------------------------------------------------------
    core::uint RendererOptions::getRenderJobCount() const
    {
        if (isRenderJobsEnabled())
        {
            if (isForcedRenderJobsCount())
                return m_renderJobsCount;
            else
                return Kernel::getScheduler()->GetWorkerThreadCount();                
        }
        else
        {
            return 0;
        }
    }
}