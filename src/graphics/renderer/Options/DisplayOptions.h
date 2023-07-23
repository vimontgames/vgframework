#pragma once

#include "core/Object/Object.h"
#include "core/Singleton/Singleton.h"
#include "graphics/driver/Device/Device_consts.h"

namespace vg::core
{
    class IFactory;
}

namespace vg::graphics::renderer
{
	enum DisplayMode : core::u32
	{
		Default = 0,
        MatID,
		VSNormal,
        VSTangent,
        VSBinormal,
        VSColor,
		UV0,
        UV1,
        Albedo,
        PSNormal
	};

    class DisplayOptions : public core::Object, public core::Singleton<DisplayOptions>
    {
    public:
        using super = core::Object;

							    DisplayOptions			(const core::string & _name, core::IObject * _parent = nullptr);

        const char *		    getClassName			() const final { return "DisplayOptions"; }

        static bool			    registerClass			(core::IFactory & _factory);
        static bool			    registerProperties		(core::IClassDesc & _desc);

        void                    onPropertyChanged       (const core::IProperty & _prop) final;

        DisplayMode             getDisplayMode          () const { return m_debugDisplayMode;}

        bool				    isToolModeEnabled		() const { return m_toolMode; }
        bool				    isAABBEnabled           () const { return m_aabb; }
        bool				    isOpaqueEnabled			() const { return m_opaque; }
        bool                    isAlbedoMapsEnabled     () const { return m_albedoMaps; }
        bool                    isNormalMapsEnabled     () const { return m_normalMaps; }
        bool				    isWireframeEnabled		() const { return m_wireframe; }

        bool                    isDisplayMatIDEnabled   () const { return DisplayMode::MatID  == m_debugDisplayMode;}
        bool				    isDisplayNormalEnabled	() const { return DisplayMode::VSNormal == m_debugDisplayMode; }
		bool				    isDisplayUV0Enabled		() const { return DisplayMode::UV0    == m_debugDisplayMode; }
        
        core::float4		    getBackgroundColor		() const { return m_backgroundColor; }

        static bool			    load					(IObject * _object);
        static bool			    save					(IObject * _object);

    protected:
        void                    ApplyVsync              (const core::IProperty * _prop);

    private:
        core::float4		    m_backgroundColor       = core::float4(0, 0, 0, 0);
        bool				    m_toolMode              = true;
        bool                    m_aabb                  = false;
        graphics::driver::VSync m_VSync                 = graphics::driver::VSync::VBL_1;
        bool                    m_vsync                 = true;
        bool				    m_opaque                = true;
        bool                    m_albedoMaps            = true;
        bool                    m_normalMaps            = true;
        bool				    m_wireframe             = false;
       
		DisplayMode	m_debugDisplayMode	= DisplayMode::Default;
    };
}