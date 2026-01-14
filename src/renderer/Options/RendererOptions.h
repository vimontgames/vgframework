#pragma once

#include "renderer/IRendererOptions.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Device/Device_consts.h"
#include "gfx/Resource/Texture_consts.h"
#include "gfx/FrameGraph/FrameGraph_consts.h"
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
    vg_enum_class(vg::renderer, RenderPassFlags, core::u32,
        ZPrepass        = 0x00000001,
        Opaque          = 0x00000002,
        Transparency    = 0x00000004
    );

    vg_enum_class(vg::renderer, LightingMode, core::u8,
        Forward = 0,
        Deferred
    );

    vg_enum(vg::renderer, Quality, core::i8,
        VeryLow = 0,
        Low     = 1,
        Medium  = 2,
        High    = 3,
        VeryHigh= 4
    );

    vg_enum(vg::renderer, ShadowDefaultResolution, core::u16,
        ShadowDefaultResolution_512 = 512,
        ShadowDefaultResolution_1024 = 1024,
        ShadowDefaultResolution_2048 = 2048,
        ShadowDefaultResolution_4096 = 4096
    );

    vg_enum_class(vg::renderer, TLASMode, core::u8,
        PerView= 0,
        PerWorld
    );

     vg_enum_class(vg::renderer, AABBFlags, core::u16,
        Mesh            = 0x0001,
        ParticleSystem  = 0x0002,
        Light           = 0x0004
    );

      vg_enum_class(vg::renderer, WireframeFlags, core::u16,
        Mesh            = 0x0001,
        ParticleSystem  = 0x0002
    );

    class RendererOptions final : public IRendererOptions
    {
    public:
        VG_CLASS_DECL(RendererOptions, IRendererOptions);
                                
							    RendererOptions			                (const core::string & _name, core::IObject * _parent, bool _safeCopy = false);
                                ~RendererOptions                        ();

        static RendererOptions * get                                    () { return s_rendererOptionsSafeCopy; }

        void                    sync                                    (const RendererOptions & _other);

        // Virtual functions exposed outside renderer should return the 'engine' value                            
        const core::float4 &    GetDefaultClearColor                    () const final override { return m_defaultEnvironmentColor; }
        gfx::ITexture *         GetDefaultCubemap                       () const final override;
        float                   GetDefaultIrradianceIntensity           () const final override { return m_defaultIrradianceIntensity; }
        float                   GetDefaultSpecularReflectionIntensity   () const final override { return m_defaultSpecularReflectionIntensity; }
        gfx::ITexture *         GetBakedSpecularBRDF                    () const final override;
        bool                    IsToolModeEnabled                       () const final override { return m_toolMode; };
        bool                    IsRayTracingEnabled                     () const final override { return m_rayTracing; };

        bool                    IsShadowEnabled                         () const final override;
        core::uint2             GetShadowDefaultResolution              () const final override;

        gfx::MSAA               GetMSAA                                 () const final override;
        bool                    SetMSAA                                 (gfx::MSAA _msaa) final override;
        
        gfx::HDR                GetHDR                                  () const final override;
        bool                    SetHDR                                  (gfx::HDR _hdr) final override;

        gfx::AAPostProcess      GetAAPostProcess                        () const final override;
        bool                    SetAAPostProcess                        (gfx::AAPostProcess _aa) final override;

        DepthOfFieldMode        GetDepthOfFieldMode                     () const final override;
        bool                    SetDepthOfFieldMode                     (DepthOfFieldMode _depthOfField) final override;

        gfx::VSync              GetVSync                                () const final override;
        bool                    SetVSync                                (gfx::VSync _vsync) final override;

       void                     EnableUI2D                              (bool _enable) final override;
       bool                     IsUI2DEnabled                           () const final override;

       void                     EnableUI3D                              (bool _enable) final override;
       bool                     IsUI3DEnabled                           () const final override;

        void                    OnPropertyChanged                       (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        // Non-virtual functions for internal renderer use 
        bool				    isToolModeEnabled                       () const { return m_toolMode; }

        core::uint2             getGridSize                             () const { return m_gridSize; }
        float                   getGridScale                            () const { return m_gridScale; }
        const core::float4 &    getGridColor                            () const { return m_gridColor;}
        core::uint              getGridSubdivCount                      () const { return m_gridSubdivCount; }
        const core::float4 &    getGridSubdivColor                      () const { return m_gridSubdivColor; }

        LightingMode            getLightingMode                         () const { return m_lightingMode; }
        PBRFlags                getPBRFlags                             () const { return m_pbrFlags; }

        DisplayMode             getDisplayMode                          () const { return m_debugDisplayMode; }
        DisplayFlags            getDisplayFlags                         () const { return m_displayFlags; }

        bool				    isAnyAABBEnabled                        () const { return (AABBFlags)0x0 != m_aabb; }
        bool				    isMeshAABBEnabled                       () const { return core::asBool(AABBFlags::Mesh & m_aabb); }
        bool				    isParticleSystemAABBEnabled             () const { return core::asBool(AABBFlags::ParticleSystem & m_aabb); }
        bool				    isLightAABBEnabled                      () const { return core::asBool(AABBFlags::Light & m_aabb); }

        bool				    isAnyWireframeEnabled                   () const { return (WireframeFlags)0x0 != m_wireframe; }
        bool				    isMeshWireframeEnabled                  () const { return core::asBool(WireframeFlags::Mesh & m_wireframe); }
        bool				    isParticleSystemWireframeEnabled        () const { return core::asBool(WireframeFlags::ParticleSystem & m_wireframe); }

        bool				    isDebugUIEnabled                        () const { return m_debugUI; }
        bool 			        isParticlesEnabled                      () const { return m_particles; }
        bool                    isDebugCameraFrustumEnabled             () const { return m_debugFrustum; }
        bool                    isSplitCullingJobsEnabled               () const { return m_splitCullingViewJobs; }

        bool                    isZPrepassEnabled                       () const { return core::asBool(RenderPassFlags::ZPrepass & m_renderPassFlags); }
        bool				    isOpaqueEnabled			                () const { return core::asBool(RenderPassFlags::Opaque & m_renderPassFlags); }
        bool				    isTransparencyEnabled                   () const { return core::asBool(RenderPassFlags::Transparency & m_renderPassFlags); }

        bool                    isPostProcessEnabled                    () const { return m_postProcess; }
        bool                    isRayTracingEnabled                     () const { return m_rayTracing; }
        TLASMode                getRayTracingTLASMode                   () const { return m_rayTracingTLASMode; }
        bool                    anyRayTracingDebugDisplay               () const;

        bool                    isDisplayMatIDEnabled                   () const { return DisplayMode::Geometry_MaterialID    == m_debugDisplayMode;}
        bool				    isDisplayNormalEnabled	                () const { return DisplayMode::Geometry_VertexNormal == m_debugDisplayMode; }
		bool				    isDisplayUV0Enabled		                () const { return DisplayMode::Geometry_UV0      == m_debugDisplayMode; }

        bool                    isAlbedoMapsEnabled                     () const { return 0 != (DisplayFlags::AlbedoMap & m_displayFlags); }
        bool                    isNormalMapsEnabled                     () const { return 0 != (DisplayFlags::NormalMap & m_displayFlags); }
        
        void                    setDefaultClearColor                    (const core::float4 & _backgroundColor);
        core::float4		    getDefaultClearColor                    () const { return m_defaultEnvironmentColor; }

        void                    Update                                  () final override;

        void                    createResources                         ();
        void                    releaseResources                        ();

        Quality                 getCurrentQualityLevel                  () const;

        VG_INLINE bool          isRenderJobsEnabled                     () const;
        VG_INLINE bool          isRenderJobsSeparateKicksEnabled        () const;
        VG_INLINE bool          isForcedRenderJobsCount                 () const;
        core::uint              getRenderJobCount                       () const;
        VG_INLINE gfx::RenderJobsPolicy getRenderJobsPolicy             () const;
        VG_INLINE core::uint    getMaxRenderTotalBufferSize             () const;
        VG_INLINE core::uint    getMaxRenderMinBufferSize               () const;

    protected:
        Quality                 autodetectQualityLevel                  ();

        void                    applyVSync                              (const core::IProperty * _prop);
        void                    applyHDR                                (const core::IProperty * _prop);
        void                    applyMSAA                               (const core::IProperty * _prop);
        void                    applyQualityLevel                       (const core::IProperty * _prop);

    private:

        static inline RendererOptions * s_rendererOptionsSafeCopy = nullptr;

        bool                    m_toolMode                              = true;

        bool                    m_rayTracing                            = false;
        TLASMode                m_rayTracingTLASMode                    = TLASMode::PerView;

        bool                    m_useCustomQualityLevel                 = false;
        Quality                 m_customQualityLevel                    = Quality::Medium;
        Quality                 m_autodetectedQualityLevel              = Quality::Medium;
        Quality                 m_previousQualityLevel                  = (Quality)-1;

        core::float4		    m_defaultEnvironmentColor               = core::float4(0.1f, 0.1f, 0.1f, 1.0f);
        bool                    m_useDefaultEnvironmentCubemap          = false;
        core::IResource *       m_defaultEnvironmentCubemap             = nullptr;
        float                   m_defaultIrradianceIntensity            = 1.0f;
        float                   m_defaultSpecularReflectionIntensity    = 1.0f;

        AABBFlags               m_aabb                                  = (AABBFlags)0x0;
        WireframeFlags          m_wireframe                             = (WireframeFlags)0x0;
        bool                    m_debugFrustum                          = false;
        bool                    m_splitCullingViewJobs                  = true;
        bool                    m_particles                             = true;
        bool                    m_postProcess                           = true;

        // UI
        bool                    m_debugUI                               = false;
        bool                    m_enableUI2D                            = true;
        bool                    m_enableUI3D                            = true;

        // Grid
        core::uint2             m_gridSize                              = float2(64,64);
        float                   m_gridScale                             = 2.0f;
        core::float4            m_gridColor                             = core::float4(0.015f, 0.015f, 0.015f, 0.5f);
        core::uint              m_gridSubdivCount                       = 2;
        core::float4            m_gridSubdivColor                       = core::float4(0.015f, 0.015f, 0.015f, 0.15f);
  
        gfx::HDR                m_HDRmode                               = gfx::HDR::None;
        gfx::MSAA               m_msaa[core::enumCount<Quality>()];
        gfx::AAPostProcess      m_aaPostProcess                         = gfx::AAPostProcess::None;
        DepthOfFieldMode        m_depthOfField[core::enumCount<Quality>()];
        gfx::VSync              m_VSync                                 = gfx::VSync::VSync_1;
        LightingMode            m_lightingMode                          = LightingMode::Forward;
        bool                    m_shadows[core::enumCount<Quality>()];
        ShadowDefaultResolution m_shadowsResolution[core::enumCount<Quality>()];
        PBRFlags                m_pbrFlags                              = (PBRFlags)0x0;
        core::IResource *       m_pbrBakedBRDFTexture                   = nullptr;
        DisplayMode	            m_debugDisplayMode                      = DisplayMode::None;
        DisplayFlags            m_displayFlags                          = DisplayFlags::AlbedoMap | DisplayFlags::NormalMap | DisplayFlags::VertexColor | DisplayFlags::MaterialColor | DisplayFlags::InstanceColor;
        RenderPassFlags         m_renderPassFlags;
        bool                    m_renderJobs                            = false;
        bool                    m_forceRenderJobsCount                  = false;  
        core::u16               m_renderJobsCount                       = 1;
        gfx::RenderJobsPolicy   m_renderJobsPolicy                      = gfx::RenderJobsPolicy::RecursiveSplit;
        core::u32               m_renderJobsTotalBufferSizeInMB         = 64;
        core::u32               m_renderJobsWorkerMinBufferSizeInMB     = 4;
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
