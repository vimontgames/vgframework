#include "renderer/Precomp.h"
#include "OmniLightInstance.h"
#include "Shaders/system/lightsBuffer.hlsli"

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
    
        registerOptionalProperty(OmniLightDesc, m_useMaxRadius, m_maxRadius, "Max. Radius");
    
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
        super("Omni", nullptr, _omniLightDesc)
    {
        m_maxRadius = _omniLightDesc->m_useMaxRadius ? _omniLightDesc->m_maxRadius : -1.0f;
    }

    //--------------------------------------------------------------------------------------
    OmniLightInstance::~OmniLightInstance()
    {

    }

    //--------------------------------------------------------------------------------------
    bool OmniLightInstance::TryGetAABB(core::AABB & _aabb) const
    {
        float radius = getMaxRadius();
        _aabb = AABB(-radius, +radius);
        return true;
    }

    //--------------------------------------------------------------------------------------
    float OmniLightInstance::getMaxRadius() const
    {
        float intensity = dot(getColor().rgb, float3(0.299f, 0.587f, 0.114f)) * m_intensity;
        float maxRadius = sqrtf(intensity / (lightEps));

        if (m_maxRadius < 0.0f)
            return maxRadius;
        else 
            return min(maxRadius, m_maxRadius); 
    }

    //--------------------------------------------------------------------------------------
    bool OmniLightInstance::Cull(CullingResult * _cullingResult, View * _view)
    {
        bool visible = _view->getCameraFrustum().intersects(getMaxRadius(), getGlobalMatrix()) != FrustumTest::Outside;

        if (visible)
        {
            _cullingResult->m_output->add(GraphicInstanceListType::All, this);
            _cullingResult->m_output->add(LightType::Omni, this);

            return true;
        }

        return false;
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
                DebugDraw::get()->AddWireframeSphere(nullptr/*_world*/, getMaxRadius(), 0xFF00FFFF, getGlobalMatrix());
                break;     
        }            
    }
}