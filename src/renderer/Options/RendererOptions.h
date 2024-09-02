#pragma once

#include "renderer/IRendererOptions.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Device/Device_consts.h"
#include "Shaders/system/displaymodes.hlsli"

namespace vg::core
{
    class IFactory;
}

namespace vg::renderer
{
    enum class RenderPassFlags : core::u32
    {
        ZPrepass        = 0x00000001,
        Opaque          = 0x00000002,
        Transparency    = 0x00000004
    };

    enum class LightingMode : core::u8
    {
        Forward = 0,
        Deferred
    };

    class RendererOptions final : public IRendererOptions, public core::Singleton<RendererOptions>
    {
    public:
        VG_CLASS_DECL(RendererOptions, IRendererOptions);

							    RendererOptions			    (const core::string & _name, core::IObject * _parent = nullptr);

        bool                    IsToolModeEnabled           () const final override { return isToolModeEnabled(); };
        bool                    IsRayTracingEnabled         () const final override { return isRayTracingEnabled(); };

        void                    OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        bool				    isToolModeEnabled           () const { return m_toolMode; }

        LightingMode            getLightingMode             () const { return m_lightingMode; }

        DisplayMode             getDisplayMode              () const { return m_debugDisplayMode; }
        DisplayFlags            getDisplayFlags             () const { return m_displayFlags; }

        bool				    isAABBEnabled               () const { return m_aabb; }
        bool				    isWireframeEnabled          () const { return m_wireframe; }
        bool				    isDebugUIEnabled            () const { return m_debugUI; }

        bool                    isZPrepassEnabled           () const { return core::asBool(RenderPassFlags::ZPrepass & m_renderPassFlags); }
        bool				    isOpaqueEnabled			    () const { return core::asBool(RenderPassFlags::Opaque & m_renderPassFlags); }
        bool				    isTransparencyEnabled       () const { return core::asBool(RenderPassFlags::Transparency & m_renderPassFlags); }

        bool                    isPostProcessEnabled        () const { return m_postProcess; }
        bool                    isRayTracingEnabled         () const { return m_rayTracing; }
        bool                    anyRayTracingDebugDisplay   () const;

        bool                    isDisplayMatIDEnabled       () const { return DisplayMode::Forward_MatID    == m_debugDisplayMode;}
        bool				    isDisplayNormalEnabled	    () const { return DisplayMode::Forward_VSNormal == m_debugDisplayMode; }
		bool				    isDisplayUV0Enabled		    () const { return DisplayMode::Forward_UV0      == m_debugDisplayMode; }

        bool                    isAlbedoMapsEnabled         () const { return 0 != (DisplayFlags::AlbedoMap & m_displayFlags); }
        bool                    isNormalMapsEnabled         () const { return 0 != (DisplayFlags::NormalMap & m_displayFlags); }
        
        void                    setBackgroundColor          (const core::float4 & _backgroundColor);
        core::float4		    getBackgroundColor		    () const { return m_backgroundColor; }

        void                    update                      () const;

    protected:
        void                    ApplyVsync                  (const core::IProperty * _prop);
        void                    ApplyHDR                    (const core::IProperty * _prop);

    private:
        core::float4		    m_backgroundColor           = core::float4(0, 0, 0, 0);
        bool				    m_toolMode                  = true;
        bool                    m_aabb                      = false;
        bool                    m_vsync                     = true;
        bool				    m_wireframe                 = false;
        bool                    m_debugUI                   = false;
        bool                    m_postProcess               = true;
        bool                    m_rayTracing                = false;
        gfx::HDR                m_HDRmode                   = gfx::HDR::None;
        gfx::VSync              m_VSync                     = gfx::VSync::VSync_1;
        LightingMode            m_lightingMode              = LightingMode::Forward;
        DisplayMode	            m_debugDisplayMode          = DisplayMode::None;
        DisplayFlags            m_displayFlags              = DisplayFlags::AlbedoMap | DisplayFlags::NormalMap;
        RenderPassFlags         m_renderPassFlags;

        core::IProperty *       m_hdrProp                   = nullptr;
        core::IProperty *       m_vsyncProp                 = nullptr;
    };
}
