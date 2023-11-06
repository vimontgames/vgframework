#include "ForwardView.h"

#include "renderer/RenderPass/Render2D/Background/BackgroundPass.h"
#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthOnlyPass.h"
#include "renderer/RenderPass/RenderObjects/Forward/ForwardPass.h"
#include "renderer/RenderPass/RenderObjects/Editor/EditorPass.h"
#include "renderer/RenderPass/RayTracing/Test/TestRayTracingPass.h"
#include "renderer/RenderPass/Compute/ComputePostProcess/ComputePostProcessPass.h"
#include "renderer/RenderPass/Render2D/FinalBlit/FinalBlitPass.h"

#include "renderer/Options/DisplayOptions.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ForwardView::ForwardView(const CreateViewParams & _params) :
        View(_params)
    {
        m_depthPrePass = new DepthOnlyPass();
        m_backgroundPass = new BackgroundPass();
        m_forwardPass = new ForwardPass();
        m_editorPass = new EditorPass();
        m_testRayTracingPass = new TestRayTracingPass();
        m_computePostProcessPass = new ComputePostProcessPass();
        m_finalBlitPass = new FinalBlitPass();        
    }

    //--------------------------------------------------------------------------------------
    ForwardView::~ForwardView()
    {
        VG_SAFE_RELEASE(m_depthPrePass);
        VG_SAFE_RELEASE(m_backgroundPass);
        VG_SAFE_RELEASE(m_forwardPass);
        VG_SAFE_RELEASE(m_editorPass);
        VG_SAFE_RELEASE(m_testRayTracingPass);
        VG_SAFE_RELEASE(m_computePostProcessPass);
        VG_SAFE_RELEASE(m_finalBlitPass);        
    }

    //--------------------------------------------------------------------------------------
    void ForwardView::RegisterFrameGraph(const gfx::RenderPassContext & _renderPassContext, FrameGraph & _frameGraph)
    {
        auto * target = getRenderTarget();
        if (target)
            _frameGraph.importRenderTarget(target->getName(), target, float4(0, 0, 0, 0), FrameGraphResource::InitState::Clear);

        super::RegisterFrameGraph(_renderPassContext, _frameGraph);

        const auto options = DisplayOptions::get();

        if (options->isRayTracingEnabled())
        {
            _frameGraph.addUserPass(_renderPassContext, m_testRayTracingPass, "TestRayTracingPass");
        }
        else
        {
            _frameGraph.addUserPass(_renderPassContext, m_backgroundPass, "BackgroundPass");

            if (options->isZPrepassEnabled())
                _frameGraph.addUserPass(_renderPassContext, m_depthPrePass, "DepthPrepass");

            _frameGraph.addUserPass(_renderPassContext, m_forwardPass, "ForwardPass");

            if (_renderPassContext.m_view->IsToolmode())
                _frameGraph.addUserPass(_renderPassContext, m_editorPass, "EditorPass");

            if (options->isPostProcessEnabled())
                _frameGraph.addUserPass(_renderPassContext, m_computePostProcessPass, "ComputePostProcessPass");

            _frameGraph.addUserPass(_renderPassContext, m_finalBlitPass, "FinalBlitPass");
        }
    }
}