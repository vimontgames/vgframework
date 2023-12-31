#include "renderer/Precomp.h"
#include "DirectionalLightInstance.h"
#include "Shaders/system/lightsBuffer.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // DirectionalLightDesc
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(DirectionalLightDesc, "Directional Light Desc");

    //--------------------------------------------------------------------------------------
    bool DirectionalLightDesc::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DirectionalLightDesc::DirectionalLightDesc(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    DirectionalLightDesc::~DirectionalLightDesc()
    {

    }

    //--------------------------------------------------------------------------------------
    // DirectionalLightInstance
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(DirectionalLightInstance, "Directional Light Instance");

    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DirectionalLightInstance::DirectionalLightInstance(const DirectionalLightDesc * _directionalLightDesc) :
        super("Directional", nullptr, _directionalLightDesc)
    {
        
    }

    //--------------------------------------------------------------------------------------
    DirectionalLightInstance::~DirectionalLightInstance()
    {

    }

    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::GetAABB(AABB & _aabb) const
    {
        // Directional light has no bounding volume
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::Cull(const Frustum & _frustum, CullingResult * _cullingResult)
    {
        // Directional light is never culled
        _cullingResult->m_output->add(GraphicInstanceListType::All, this);
        _cullingResult->m_output->add(GraphicInstanceListType::Light, this);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void DirectionalLightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        
    }
}