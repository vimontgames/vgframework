#include "renderer/Precomp.h"
#include "DirectionalLightInstance.h"
#include "Shaders/system/lightsBuffer.hlsli"
#include "renderer/View/Shadow/ShadowView.h"
#include "core/Scheduler/Scheduler.h"

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
    bool DirectionalLightInstance::TryGetAABB(AABB & _aabb) const
    {
        // Directional light has no bounding volume
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool DirectionalLightInstance::Cull(CullingResult * _cullingResult, View * _view)
    {
        // Directional light is never culled
        _cullingResult->m_output->add(GraphicInstanceListType::All, this);
        _cullingResult->m_output->add(LightType::Directional, this);

        if (!_view->isAdditionalView() && IsCastShadow())
        {
            // Create view and start culling immediately
            Renderer * renderer = Renderer::get();

            ShadowView * shadowView = new ShadowView(this, _view->getWorld(), m_shadowResolution);
            _view->addShadowView(shadowView);

            shadowView->SetupOrthographicCamera(this->getGlobalMatrix(), m_shadowSize, m_shadowRange);

            _cullingResult->m_output->add(shadowView);

            core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();
            jobScheduler->Start(shadowView->getCullingJob(), renderer->GetJobSync(RendererJobType::Culling));
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    void DirectionalLightInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        
    }
}