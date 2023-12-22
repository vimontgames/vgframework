#include "ForwardView.h"

#include "renderer/RenderPass/Render2D/Background/BackgroundPass.h"
#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthOnlyPass.h"
#include "renderer/RenderPass/RenderObjects/Forward/ForwardPass.h"
#include "renderer/RenderPass/RenderObjects/Editor/EditorPass.h"
#include "renderer/RenderPass/Compute/ComputePostProcess/ComputePostProcessPass.h"
#include "renderer/RenderPass/Render2D/FinalBlit/FinalBlitPass.h"
#include "renderer/RenderPass/Update/TLAS/TLASUpdatePass.h"

#include "renderer/Options/RendererOptions.h"

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
        m_computePostProcessPass = new ComputePostProcessPass();
        m_finalBlitPass = new FinalBlitPass();  
        m_TLASUpdatePass = new TLASUpdatePass();
    }

    //--------------------------------------------------------------------------------------
    ForwardView::~ForwardView()
    {
        VG_SAFE_RELEASE(m_depthPrePass);
        VG_SAFE_RELEASE(m_backgroundPass);
        VG_SAFE_RELEASE(m_forwardPass);
        VG_SAFE_RELEASE(m_editorPass);
        VG_SAFE_RELEASE(m_computePostProcessPass);
        VG_SAFE_RELEASE(m_finalBlitPass);   
        VG_SAFE_RELEASE(m_TLASUpdatePass);
    }

    //--------------------------------------------------------------------------------------
    bool ForwardView::IsUsingRayTracing() const
    {
        return RendererOptions::get()->isRayTracingEnabled();
    }

    //--------------------------------------------------------------------------------------
    void ForwardView::RegisterFrameGraph(const gfx::RenderPassContext & _renderPassContext, FrameGraph & _frameGraph)
    {
        auto * target = getRenderTarget();
        if (target)
            _frameGraph.importRenderTarget(target->getName(), target, float4(0, 0, 0, 0), FrameGraphResource::InitState::Clear);

        super::RegisterFrameGraph(_renderPassContext, _frameGraph);

        const RendererOptions * options = RendererOptions::get();

        const bool toolmode = _renderPassContext.m_view->IsToolmode();
        const bool computePostProcess = _renderPassContext.m_view->IsComputePostProcessNeeded();

        _frameGraph.addUserPass(_renderPassContext, m_backgroundPass, "BackgroundPass");

        if (_renderPassContext.m_view->IsUsingRayTracing())
            _frameGraph.addUserPass(_renderPassContext, m_TLASUpdatePass, "TLASUpdatePass");

        if (options->isZPrepassEnabled())
            _frameGraph.addUserPass(_renderPassContext, m_depthPrePass, "DepthPrepass");

        _frameGraph.addUserPass(_renderPassContext, m_forwardPass, "ForwardPass");

        if (toolmode)
            _frameGraph.addUserPass(_renderPassContext, m_editorPass, "EditorPass");

        if (_renderPassContext.m_view->IsComputePostProcessNeeded())
            _frameGraph.addUserPass(_renderPassContext, m_computePostProcessPass, "ComputePostProcessPass");

        _frameGraph.addUserPass(_renderPassContext, m_finalBlitPass, "FinalBlitPass");
    }
}