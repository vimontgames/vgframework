#pragma once

#include "renderer/IDisplayOptions.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Device/Device_consts.h"
#include "Shaders/system/displayoptions.hlsli"

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
        Transparency    = 0x00000004,
        PostProcess     = 0x00000008,
        RayTracing      = 0x00000010
    };

    class DisplayOptions final : public IDisplayOptions, public core::Singleton<DisplayOptions>
    {
    public:
        VG_CLASS_DECL(DisplayOptions, IDisplayOptions);

							    DisplayOptions			    (const core::string & _name, core::IObject * _parent = nullptr);

        void                    OnPropertyChanged           (IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;

        bool                    IsCollidersEnabled          () const final override { return isCollidersEnabled(); }

        DisplayMode             getDisplayMode              () const { return m_debugDisplayMode; }
        DisplayFlags            getDisplayFlags             () const { return m_displayFlags; }

        bool				    isToolModeEnabled		    () const { return m_toolMode; }

        bool				    isAABBEnabled               () const { return m_aabb; }
        bool				    isWireframeEnabled          () const { return m_wireframe; }
        bool                    isCollidersEnabled          () const { return m_colliders;}

        bool                    isZPrepassEnabled           () const { return core::asBool(RenderPassFlags::ZPrepass & m_renderPassFlags); }
        bool				    isOpaqueEnabled			    () const { return core::asBool(RenderPassFlags::Opaque & m_renderPassFlags); }
        bool                    isPostProcessEnabled        () const { return core::asBool(RenderPassFlags::PostProcess & m_renderPassFlags); }
        bool                    isRayTracingEnabled         () const { return core::asBool(RenderPassFlags::RayTracing & m_renderPassFlags); }

        bool                    isDisplayMatIDEnabled       () const { return DisplayMode::MatID    == m_debugDisplayMode;}
        bool				    isDisplayNormalEnabled	    () const { return DisplayMode::VSNormal == m_debugDisplayMode; }
		bool				    isDisplayUV0Enabled		    () const { return DisplayMode::UV0      == m_debugDisplayMode; }

        bool                    isAlbedoMapsEnabled         () const { return 0 != (DisplayFlags::AlbedoMap & m_debugDisplayMode); }
        bool                    isNormalMapsEnabled         () const { return 0 != (DisplayFlags::NormalMap & m_debugDisplayMode); }
        
        void                    setBackgroundColor          (const core::float4 & _backgroundColor);
        core::float4		    getBackgroundColor		    () const { return m_backgroundColor; }

        static bool			    load					    (IObject * _object);
        static bool			    save					    (IObject * _object);

    protected:
        void                    ApplyVsync                  (const core::IProperty * _prop);

    private:
        core::float4		    m_backgroundColor           = core::float4(0, 0, 0, 0);
        bool				    m_toolMode                  = true;
        bool                    m_aabb                      = false;
        gfx::VSync m_VSync                                  = gfx::VSync::VBL_1;
        bool                    m_vsync                     = true;
        bool				    m_wireframe                 = false;
        bool                    m_colliders                 = false;
       
        DisplayMode	            m_debugDisplayMode;
        DisplayFlags            m_displayFlags;
        RenderPassFlags         m_renderPassFlags;
    };
}