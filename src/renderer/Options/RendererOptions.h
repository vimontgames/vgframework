#pragma once

#include "renderer/IRendererOptions.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Device/Device_consts.h"
#include "gfx/Resource/Texture_consts.h"
#include "Shaders/system/displaymodes.hlsli"
#include "renderer/PBR/PBR_Consts.h"

namespace vg::core
{
    class IFactory;
}

namespace vg::gfx
{
    class DeviceCaps;
}

namespace vg::renderer
{
    vg_enum_class(RenderPassFlags, core::u32,
        ZPrepass        = 0x00000001,
        Opaque          = 0x00000002,
        Transparency    = 0x00000004
    );

    vg_enum_class(LightingMode, core::u8,
        Forward = 0,
        Deferred
    );

    vg_enum(Quality, core::i8,
        VeryLow = 0,
        Low     = 1,
        Medium  = 2,
        High    = 3,
        VeryHigh= 4
    );

    vg_enum(ShadowDefaultResolution, core::u16,
        ShadowDefaultResolution_512 = 512,
        ShadowDefaultResolution_1024 = 1024,
        ShadowDefaultResolution_2048 = 2048,
        ShadowDefaultResolution_4096 = 4096
    );

    class RendererOptions final : public IRendererOptions, public core::Singleton<RendererOptions>
    {
    public:
        VG_CLASS_DECL(RendererOptions, IRendererOptions);

							    RendererOptions			                (const core::string & _name, core::IObject * _parent = nullptr);
                                ~RendererOptions                        ();

        const core::float4 &    GetDefaultClearColor                    () const final override { return m_defaultEnvironmentColor; }
        gfx::ITexture *         GetDefaultCubemap                       () const final override;
        float                   GetDefaultIrradianceIntensity           () const final override { return m_defaultIrradianceIntensity; }
        float                   GetDefaultSpecularReflectionIntensity   () const final override { return m_defaultSpecularReflectionIntensity; }

        bool                    IsToolModeEnabled                       () const final override { return isToolModeEnabled(); };
        bool                    IsRayTracingEnabled                     () const final override { return isRayTracingEnabled(); };

        bool                    GetShadowsEnabled                       () const final override;
        core::uint2             GetShadowDefaultResolution              () const final override;

        gfx::MSAA               GetMSAA                                 () const final override;
        bool                    SetMSAA                                 (gfx::MSAA _msaa) final override;
        
        gfx::HDR                GetHDR                                  () const final override;
        bool                    SetHDR                                  (gfx::HDR _hdr) final override;

        gfx::AAPostProcess      GetAAPostProcess                        () const final override;
        bool                    SetAAPostProcess                        (gfx::AAPostProcess _aa) final override;

        gfx::VSync              GetVSync                                () const final override;
        bool                    SetVSync                                (gfx::VSync _vsync) final override;

        void                    OnPropertyChanged                       (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        bool				    isToolModeEnabled                       () const { return m_toolMode; }

        LightingMode            getLightingMode                         () const { return m_lightingMode; }
        PBRFlags                getPBRFlags                             () const { return m_pbrFlags; }

        DisplayMode             getDisplayMode                          () const { return m_debugDisplayMode; }
        DisplayFlags            getDisplayFlags                         () const { return m_displayFlags; }

        bool				    isAABBEnabled                           () const { return m_aabb; }
        bool				    isWireframeEnabled                      () const { return m_wireframe; }
        bool				    isDebugUIEnabled                        () const { return m_debugUI; }

        bool                    isZPrepassEnabled                       () const { return core::asBool(RenderPassFlags::ZPrepass & m_renderPassFlags); }
        bool				    isOpaqueEnabled			                () const { return core::asBool(RenderPassFlags::Opaque & m_renderPassFlags); }
        bool				    isTransparencyEnabled                   () const { return core::asBool(RenderPassFlags::Transparency & m_renderPassFlags); }

        bool                    isPostProcessEnabled                    () const { return m_postProcess; }
        bool                    isRayTracingEnabled                     () const { return m_rayTracing; }
        bool                    anyRayTracingDebugDisplay               () const;

        bool                    isDisplayMatIDEnabled                   () const { return DisplayMode::Geometry_MaterialID    == m_debugDisplayMode;}
        bool				    isDisplayNormalEnabled	                () const { return DisplayMode::Geometry_VertexNormal == m_debugDisplayMode; }
		bool				    isDisplayUV0Enabled		                () const { return DisplayMode::Geometry_UV0      == m_debugDisplayMode; }

        bool                    isAlbedoMapsEnabled                     () const { return 0 != (DisplayFlags::AlbedoMap & m_displayFlags); }
        bool                    isNormalMapsEnabled                     () const { return 0 != (DisplayFlags::NormalMap & m_displayFlags); }
        
        void                    setDefaultClearColor                    (const core::float4 & _backgroundColor);
        core::float4		    getDefaultClearColor                    () const { return m_defaultEnvironmentColor; }

        void                    update                                  () const;

        void                    createResources                         ();
        void                    releaseResources                        ();

        Quality                 getCurrentQualityLevel                  () const;

        VG_INLINE bool          isRenderJobsEnabled                     () const;

    protected:
        Quality                 autodetectQualityLevel                  ();

        void                    applyVSync                              (const core::IProperty * _prop);
        void                    applyHDR                                (const core::IProperty * _prop);
        void                    applyMSAA                               (const core::IProperty * _prop);
        void                    applyQualityLevel                       (const core::IProperty * _prop);

    private:
        bool                    m_useCustomQualityLevel                 = false;
        Quality                 m_customQualityLevel                    = Quality::Medium;
        Quality                 m_autodetectedQualityLevel              = Quality::Medium;
        Quality                 m_previousQualityLevel                  = (Quality)-1;

        core::float4		    m_defaultEnvironmentColor               = core::float4(0.1f, 0.1f, 0.1f, 1.0f);
        bool                    m_useDefaultEnvironmentCubemap          = false;
        core::IResource *       m_defaultEnvironmentCubemap             = nullptr;
        float                   m_defaultIrradianceIntensity            = 1.0f;
        float                   m_defaultSpecularReflectionIntensity    = 1.0f;
        bool				    m_toolMode                              = true;
        bool                    m_aabb                                  = false;
        bool				    m_wireframe                             = false;
        bool                    m_debugUI                               = false;
        bool                    m_postProcess                           = true;
        bool                    m_rayTracing                            = false;
        gfx::HDR                m_HDRmode                               = gfx::HDR::None;
        gfx::MSAA               m_msaa[core::enumCount<Quality>()];
        gfx::AAPostProcess      m_aaPostProcess                         = gfx::AAPostProcess::None;
        gfx::VSync              m_VSync                                 = gfx::VSync::VSync_1;
        LightingMode            m_lightingMode                          = LightingMode::Forward;
        bool                    m_shadows[core::enumCount<Quality>()];
        ShadowDefaultResolution m_shadowsResolution[core::enumCount<Quality>()];
        PBRFlags                m_pbrFlags                              = (PBRFlags)0x0;
        DisplayMode	            m_debugDisplayMode                      = DisplayMode::None;
        DisplayFlags            m_displayFlags                          = DisplayFlags::AlbedoMap | DisplayFlags::NormalMap | DisplayFlags::VertexColor | DisplayFlags::MaterialColor | DisplayFlags::InstanceColor;
        RenderPassFlags         m_renderPassFlags;
        bool                    m_renderJobs                            = false;
        const gfx::DeviceCaps * m_deviceCaps                            = nullptr;
        
        core::IProperty *       m_hdrProp                               = nullptr;
        core::IProperty *       m_vsyncProp                             = nullptr;
        core::IProperty *       m_msaaProp[core::enumCount<Quality>()]  = {};
        core::IProperty *       m_aaPostProcessProp                     = nullptr;
    };
}

#if VG_ENABLE_INLINE
#include "RendererOptions.inl"
#endif
