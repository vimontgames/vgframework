#include "ForwardView.h"

#include "graphics/renderer/Pass/Background/BackgroundPass.h"
#include "graphics/renderer/Pass/Forward/ForwardPass.h"
#include "graphics/renderer/Pass/PostProcess/PostProcessPass.h"

using namespace vg::core;
using namespace vg::graphics::driver;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ForwardView::ForwardView(const CreateViewParams & _params) :
        View(_params)
    {
        m_backgroundPass = new BackgroundPass();
        m_forwardPass = new ForwardPass();
        m_postProcessPass = new PostProcessPass();
    }

    //--------------------------------------------------------------------------------------
    ForwardView::~ForwardView()
    {
        VG_SAFE_RELEASE(m_backgroundPass);
        VG_SAFE_RELEASE(m_forwardPass);
        VG_SAFE_RELEASE(m_postProcessPass);
    }


    //--------------------------------------------------------------------------------------
    void ForwardView::AddToFrameGraph(FrameGraph & _frameGraph)
    {
        driver::RenderContext rc;
                              rc.m_view = this;

        auto * target = getRenderTarget();
        if (target)
            _frameGraph.importRenderTarget(target->getName(), target, float4(0, 0, 0, 0), FrameGraph::Resource::InitState::Clear);

        _frameGraph.addUserPass(rc, m_backgroundPass, "BackgroundPass");
        _frameGraph.addUserPass(rc, m_forwardPass, "ForwardPass");
        _frameGraph.addUserPass(rc, m_postProcessPass, "PostProcessPass");
    }
}