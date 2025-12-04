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
    
        registerOptionalProperty(OmniLightDesc, m_useMaxRadius, m_maxRadius, "Radius");
        setPropertyRange(OmniLightDesc, m_maxRadius, float2(0.01f, 256.0f));
    
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
        float intensity = dot(getColor().rgb, lightLuminance) * m_intensity;
        float maxRadius = sqrtf(intensity / (lightEps));

        if (m_maxRadius < 0.0f)
            return maxRadius;
        else 
            return min(maxRadius, m_maxRadius); 
    }

    //--------------------------------------------------------------------------------------
    bool OmniLightInstance::Cull(const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult)
    {
        bool visible = _cullingOptions.m_view->getCameraFrustum().intersects(getMaxRadius(), getGlobalMatrix()) != FrustumTest::Outside;

        if (visible)
        {
            super::Cull(_cullingOptions, _cullingResult);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void OmniLightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        //super::Draw(_renderContext, _cmdList);

        switch (_renderContext.m_shaderPass)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderContext.m_shaderPass);
                break;

            case ShaderPass::ZOnly:
                break;

            case ShaderPass::Outline:
            case ShaderPass::Forward:
            case ShaderPass::Deferred:
            {
                const float4x4 & mGlobal = getGlobalMatrix();
                auto * debugDraw = DebugDraw::get();
                const auto color = packRGBA8(GetColor());
                const auto pickingID = GetPickingID();
                const float radius = 0.25f;

                if (ShaderPass::Outline == _renderContext.m_shaderPass)
                {
                    debugDraw->drawSphere(_cmdList, DebugDrawFillMode::Outline, true, radius, color, mGlobal, pickingID);
                }
                else
                {
                    debugDraw->drawSphere(_cmdList, DebugDrawFillMode::Solid, false, radius, color, mGlobal, pickingID);
                    debugDraw->drawSphere(_cmdList, DebugDrawFillMode::Solid, true, radius, color, mGlobal, pickingID);
                    //debugDraw->AddWireframeSphere(_renderContext.m_renderPass->getWorld(), getMaxRadius(), color, mGlobal, pickingID);
                }                
            }
            break;
        }            
    }
}