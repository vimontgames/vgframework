#include "renderer/Precomp.h"
#include "OmniLightInstance.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // OmniLightDesc
    //--------------------------------------------------------------------------------------
    
    VG_REGISTER_OBJECT_CLASS(OmniLightDesc, "Omni Light Desc");

    //--------------------------------------------------------------------------------------
    bool OmniLightDesc::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
    
        registerProperty(OmniLightDesc, m_radius, "Radius");
    
        return true;
    }
    
    //--------------------------------------------------------------------------------------
    OmniLightDesc::OmniLightDesc(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
    
    }
    
    //--------------------------------------------------------------------------------------
    OmniLightDesc::~OmniLightDesc()
    {
        
    }

    //--------------------------------------------------------------------------------------
    // OmniLightInstance
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(OmniLightInstance, "Omni Light Instance");

    //--------------------------------------------------------------------------------------
    bool OmniLightInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    OmniLightInstance::OmniLightInstance(const OmniLightDesc * _omniLightDesc) :
        super("Omni", nullptr)
    {
        m_radius = _omniLightDesc->m_radius;
    }

    //--------------------------------------------------------------------------------------
    OmniLightInstance::~OmniLightInstance()
    {

    }

    //--------------------------------------------------------------------------------------
    void OmniLightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        DebugDraw::get()->AddWireframeSphere(m_radius, 0xFF00FFFF, getWorldMatrix());
    }
}