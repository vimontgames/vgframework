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
    bool OmniLightInstance::GetAABB(AABB & _aabb) const
    {
        _aabb = AABB(-m_radius, +m_radius);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void OmniLightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
                break;

            case ShaderPass::Forward:
            case ShaderPass::Deferred:
                DebugDraw::get()->AddWireframeSphere(m_radius, 0xFF00FFFF, getGlobalMatrix());
                break;     
        }            
    }
}