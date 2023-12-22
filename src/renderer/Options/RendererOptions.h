#pragma once

#include "renderer/IRendererOptions.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Device/Device_consts.h"
#include "Shaders/system/options.hlsli"

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

    class RendererOptions final : public IRendererOptions, public core::Singleton<RendererOptions>
    {
    public:
        VG_CLASS_DECL(RendererOptions, IRendererOptions);

							    RendererOptions			    (const core::string & _name, core::IObject * _parent = nullptr);

        void                    OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        bool				    isToolModeEnabled           () const { return m_toolMode; }

        DisplayMode             getDisplayMode              () const { return m_debugDisplayMode; }
        DisplayFlags            getDisplayFlags             () const { return m_displayFlags; }

        RayTracingMode          getRayTracingMode           () const { return m_rayTracing? m_rayTracingMode : RayTracingMode::Default; }
        PostProcessMode         getPostProcessMode          () const { return m_postProcess? m_postProcessMode : PostProcessMode::Default; }

        bool				    isAABBEnabled               () const { return m_aabb; }
        bool				    isWireframeEnabled          () const { return m_wireframe; }

        bool                    isZPrepassEnabled           () const { return core::asBool(RenderPassFlags::ZPrepass & m_renderPassFlags); }
        bool				    isOpaqueEnabled			    () const { return core::asBool(RenderPassFlags::Opaque & m_renderPassFlags); }
        bool				    isTransparencyEnabled       () const { return core::asBool(RenderPassFlags::Transparency & m_renderPassFlags); }

        bool                    isPostProcessEnabled        () const { return m_postProcess; }
        bool                    isRayTracingEnabled         () const { return m_rayTracing; }

        bool                    isDisplayMatIDEnabled       () const { return DisplayMode::MatID    == m_debugDisplayMode;}
        bool				    isDisplayNormalEnabled	    () const { return DisplayMode::VSNormal == m_debugDisplayMode; }
		bool				    isDisplayUV0Enabled		    () const { return DisplayMode::UV0      == m_debugDisplayMode; }

        bool                    isAlbedoMapsEnabled         () const { return 0 != (DisplayFlags::AlbedoMap & m_displayFlags); }
        bool                    isNormalMapsEnabled         () const { return 0 != (DisplayFlags::NormalMap & m_displayFlags); }
        
        void                    setBackgroundColor          (const core::float4 & _backgroundColor);
        core::float4		    getBackgroundColor		    () const { return m_backgroundColor; }

    protected:
        void                    ApplyVsync                  (const core::IProperty * _prop);

    private:
        core::float4		    m_backgroundColor           = core::float4(0, 0, 0, 0);
        bool				    m_toolMode                  = true;
        bool                    m_aabb                      = false;
        gfx::VSync              m_VSync                     = gfx::VSync::VBL_1;
        bool                    m_vsync                     = true;
        bool				    m_wireframe                 = false;

        DisplayMode	            m_debugDisplayMode          = DisplayMode::Default;
        DisplayFlags            m_displayFlags              = DisplayFlags::AlbedoMap | DisplayFlags::NormalMap;

        bool                    m_postProcess               = true;
        PostProcessMode         m_postProcessMode           = PostProcessMode::Default;

        bool                    m_rayTracing                = false;
        RayTracingMode          m_rayTracingMode            = RayTracingMode::Default;

        RenderPassFlags         m_renderPassFlags;
    };
}
