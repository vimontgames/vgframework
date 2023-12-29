#include "LitView.h"

#include "renderer/RenderPass/Render2D/Background/BackgroundPass.h"
#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthOnlyPass.h"
#include "renderer/RenderPass/RenderObjects/Forward/ForwardOpaquePass.h"
#include "renderer/RenderPass/RenderObjects/Deferred/DeferredOpaquePass.h"
#include "renderer/RenderPass/RenderObjects/Editor/EditorPass.h"
#include "renderer/RenderPass/Compute/ComputePostProcess/ComputePostProcessPass.h"
#include "renderer/RenderPass/Compute/ComputeDeferredLighting/ComputeDeferredLightingPass.h"
#include "renderer/RenderPass/Render2D/FinalBlit/FinalBlitPass.h"
#include "renderer/RenderPass/Update/TLAS/TLASUpdatePass.h"

#include "renderer/Options/RendererOptions.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    LitView::LitView(const CreateViewParams & _params) :
        View(_params)
    {
        m_depthPrePass = new DepthOnlyPass();
        m_backgroundPass = new BackgroundPass();
        m_forwardOpaquePass = new ForwardOpaquePass();
        m_deferredOpaquePass = new DeferredOpaquePass();
        m_deferredLightingPass = new ComputeDeferredLightingPass();
        m_editorPass = new EditorPass();
        m_computePostProcessPass = new ComputePostProcessPass();
        m_finalBlitPass = new FinalBlitPass();  
        m_TLASUpdatePass = new TLASUpdatePass();
    }

    //--------------------------------------------------------------------------------------
    LitView::~LitView()
    {
        VG_SAFE_RELEASE(m_depthPrePass);
        VG_SAFE_RELEASE(m_backgroundPass);
        VG_SAFE_RELEASE(m_forwardOpaquePass);
        VG_SAFE_RELEASE(m_deferredOpaquePass);
        VG_SAFE_RELEASE(m_deferredLightingPass);
        VG_SAFE_RELEASE(m_editorPass);
        VG_SAFE_RELEASE(m_computePostProcessPass);
        VG_SAFE_RELEASE(m_finalBlitPass);   
        VG_SAFE_RELEASE(m_TLASUpdatePass);
    }

    //--------------------------------------------------------------------------------------
    bool LitView::IsUsingRayTracing() const
    {
        return RendererOptions::get()->isRayTracingEnabled();
    }

    //--------------------------------------------------------------------------------------
    void LitView::RegisterFrameGraph(const gfx::RenderPassContext & _renderPassContext, FrameGraph & _frameGraph)
    {
        auto * target = getRenderTarget();
        if (target)
            _frameGraph.importRenderTarget(target->getName(), target, float4(0, 0, 0, 0), FrameGraphResource::InitState::Clear);

        super::RegisterFrameGraph(_renderPassContext, _frameGraph);

        const RendererOptions * options = RendererOptions::get();

        const bool toolmode = _renderPassContext.m_view->IsToolmode();
        const bool computePostProcess = _renderPassContext.m_view->IsComputePostProcessNeeded();

        if (_renderPassContext.m_view->IsUsingRayTracing())
            _frameGraph.addUserPass(_renderPassContext, m_TLASUpdatePass, "TLAS Update");

        // Because the ZPrepass is optional, the "Background" pass will always initialize the buffers used during opaque
        _frameGraph.addUserPass(_renderPassContext, m_backgroundPass, "Background");

        if (options->isZPrepassEnabled())
            _frameGraph.addUserPass(_renderPassContext, m_depthPrePass, "ZPrepass");

        switch (options->getLightingMode())
        {
            case LightingMode::Forward:
                // Write directly to "Color"
                _frameGraph.addUserPass(_renderPassContext, m_forwardOpaquePass, "ForwardOpaque");
                break;

            case LightingMode::Deferred:
                // Write to GBuffers ...
                _frameGraph.addUserPass(_renderPassContext, m_deferredOpaquePass, "DeferredOpaque");

                // Then compute deferred shading to "Color" ("Color" must be an UAV *and* RenderTarget)
                _frameGraph.addUserPass(_renderPassContext, m_deferredLightingPass, "DeferredLightingPass");
                break;
        }       

        // Render editor display to "Color"
        if (toolmode)
            _frameGraph.addUserPass(_renderPassContext, m_editorPass, "Editor");

        // Apply PostProcess from "Color" and "DepthStencil" to "PostProcessUAV"
        if (_renderPassContext.m_view->IsComputePostProcessNeeded())
            _frameGraph.addUserPass(_renderPassContext, m_computePostProcessPass, "PostProcess");

        // Read from "Color" (postprocess OFF) or "PostProcessUAV" (postprocess ON) to final dest
        _frameGraph.addUserPass(_renderPassContext, m_finalBlitPass, "Final Blit");
    }
}