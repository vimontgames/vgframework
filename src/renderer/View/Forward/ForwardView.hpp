#include "ForwardView.h"

#include "renderer/RenderPass/Render2D/Background/BackgroundPass.h"
#include "renderer/RenderPass/Render2D/FinalPostProcess/FinalPostProcessPass.h"
#include "renderer/RenderPass/RenderObjects/Forward/ForwardPass.h"
#include "renderer/RenderPass/RayTracing/Test/TestRayTracingPass.h"

#include "renderer/Options/DisplayOptions.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ForwardView::ForwardView(const CreateViewParams & _params) :
        View(_params)
    {
        m_backgroundPass = new BackgroundPass();
        m_forwardPass = new ForwardPass();
        m_postProcessPass = new FinalPostProcessPass();

        m_testRayTracingPass = new TestRayTracingPass();
    }

    //--------------------------------------------------------------------------------------
    ForwardView::~ForwardView()
    {
        VG_SAFE_RELEASE(m_backgroundPass);
        VG_SAFE_RELEASE(m_forwardPass);
        VG_SAFE_RELEASE(m_postProcessPass);

        VG_SAFE_RELEASE(m_testRayTracingPass);
    }


    //--------------------------------------------------------------------------------------
    void ForwardView::AddToFrameGraph(FrameGraph & _frameGraph)
    {
        gfx::RenderPassContext rc;
                              rc.m_view = this;

        auto * target = getRenderTarget();
        if (target)
            _frameGraph.importRenderTarget(target->getName(), target, float4(0, 0, 0, 0), FrameGraph::Resource::InitState::Clear);

        const auto options = DisplayOptions::get();
        if (options->isRayTracingEnabled())
        {
            _frameGraph.addUserPass(rc, m_testRayTracingPass, "TestRayTracingPass");
        }
        else
        {
            _frameGraph.addUserPass(rc, m_backgroundPass, "BackgroundPass");
            _frameGraph.addUserPass(rc, m_forwardPass, "ForwardPass");
            _frameGraph.addUserPass(rc, m_postProcessPass, "PostProcessPass");
        }
    }
}