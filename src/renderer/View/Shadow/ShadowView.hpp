#include "ShadowView.h"
#include "renderer/Instance/Light/LightInstance.h"

#include "renderer/RenderPass/RenderObjects/DepthOnly/ShadowMap/ShadowMapPass.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ShadowView::ShadowView(LightInstance * _light, core::IWorld * _world, core::uint2 _resolution) :
        View(CreateViewParams(ViewTarget::Shadow, _resolution, _world)),
        m_light(_light)
    {
        setIsAdditionalView(true);
        setOrtho(true);
        m_shadowMapPass = new ShadowMapPass();
    }

    //--------------------------------------------------------------------------------------
    ShadowView::~ShadowView()
    {
        VG_SAFE_RELEASE(m_shadowMapPass);
    }

    //--------------------------------------------------------------------------------------
    bool ShadowView::isUsingShadowMap() const
    {
        if (m_light)
            return m_light->m_shadow && !RendererOptions::get()->IsRayTracingEnabled();

        return false;
    }

    //--------------------------------------------------------------------------------------
    core::string ShadowView::getShadowMapName(const IView * _view) const
    {
        VG_ASSERT(isUsingShadowMap());

        RenderPassContext rc;
        rc.m_view = (IView *)this;
        rc.m_parent = (IView *)_view;

        return rc.getFrameGraphIDEx("ShadowMap");
    }

    //--------------------------------------------------------------------------------------
    void ShadowView::RegisterFrameGraph(const gfx::RenderPassContext & _renderPassContext, FrameGraph & _frameGraph)
    {
        VG_ASSERT(m_light && m_light->IsCastShadow());

        if (isUsingShadowMap())
        {
            _frameGraph.pushPassGroup(fmt::sprintf("Shadow #%u", _renderPassContext.m_view->GetViewID().index));

            gfx::RenderPassContext rc = _renderPassContext;
            rc.m_view = this;
            rc.m_parent = _renderPassContext.m_view;

            super::RegisterFrameGraph(rc, _frameGraph);

            _frameGraph.addUserPass(rc, m_shadowMapPass, "ShadowMap");

            _frameGraph.popPassGroup();
        }
    }
}