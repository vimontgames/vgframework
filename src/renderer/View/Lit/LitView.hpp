#include "LitView.h"
#include "renderer/View/Shadow/ShadowView.h"
#include "renderer/RenderPass/Render2D/Background/BackgroundPass.h"
#include "renderer/RenderPass/RenderObjects/DepthOnly/DepthPrepass/DepthPrePass.h"
#include "renderer/RenderPass/RenderObjects/Forward/ForwardOpaquePass.h"
#include "renderer/RenderPass/RenderObjects/Forward/ForwardTransparentPass.h"
#include "renderer/RenderPass/RenderObjects/Deferred/DeferredOpaquePass.h"
#include "renderer/RenderPass/RenderObjects/Editor/EditorPass.h"
#include "renderer/RenderPass/Compute/ComputePostProcess/ComputePostProcessPass.h"
#include "renderer/RenderPass/Compute/ComputeDeferredLighting/ComputeDeferredLightingPass.h"
#include "renderer/RenderPass/Render2D/FinalBlit/FinalBlitPass.h"
#include "renderer/RenderPass/Update/TLAS/TLASUpdatePass.h"
#include "renderer/Renderer.h"
#include "renderer/Options/RendererOptions.h"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    LitView::LitView(const CreateViewParams & _params) :
        View(_params)
    {
        m_depthPrePass = new DepthPrePass();
        m_backgroundPass = new BackgroundPass();
        m_forwardOpaquePass = new ForwardOpaquePass();
        m_forwardTransparentPass = new ForwardTransparentPass();
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
        VG_SAFE_RELEASE(m_forwardTransparentPass);
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
        _frameGraph.pushPassGroup(_renderPassContext.m_view->getName());

        // If the view does not belong to a viewport then it must register its target
        if (!GetViewport())
        {
            auto * target = getRenderTarget();
            if (target)
                _frameGraph.importRenderTarget(target->getName(), target, float4(0, 0, 0, 1), FrameGraphResource::InitState::Clear);
        }

        const RendererOptions * options = RendererOptions::get();

        const bool toolmode = _renderPassContext.m_view->IsToolmode();
        const bool computePostProcess = _renderPassContext.m_view->IsComputePostProcessNeeded();

        if (_renderPassContext.m_view->IsUsingRayTracing())
            _frameGraph.addUserPass(_renderPassContext, m_TLASUpdatePass, "TLAS Update");

        // Render shadow maps
        {
            _frameGraph.pushPassGroup("Shadows");

            auto shadowViews = getShadowViews();
            for (uint i = 0; i < shadowViews.size(); ++i)
            {
                auto * shadowView = shadowViews[i];

                // TODO: render only shadows updated this frame?
                //if (shadowView->IsRender())
                    shadowView->RegisterFrameGraph(_renderPassContext, _frameGraph);
            }

            _frameGraph.popPassGroup();
        }

        // Update view constants after rendering shadowmaps
        super::RegisterFrameGraph(_renderPassContext, _frameGraph);

        // Because the ZPrepass is optional, the "Background" pass will always initialize the buffers used during opaque
        _frameGraph.addUserPass(_renderPassContext, m_backgroundPass, "Background");

        if (options->isZPrepassEnabled())
            _frameGraph.addUserPass(_renderPassContext, m_depthPrePass, "DepthPrepass");

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

        _frameGraph.addUserPass(_renderPassContext, m_forwardTransparentPass, "ForwardTransparent");

        // Render editor display to "Color"
        if (toolmode)
            _frameGraph.addUserPass(_renderPassContext, m_editorPass, "Editor");

        // Apply PostProcess from "Color" and "DepthStencil" to "PostProcessUAV"
        if (_renderPassContext.m_view->IsComputePostProcessNeeded())
            _frameGraph.addUserPass(_renderPassContext, m_computePostProcessPass, "PostProcess");

        // Read from "Color" (postprocess OFF) or "PostProcessUAV" (postprocess ON) to final dest
        _frameGraph.addUserPass(_renderPassContext, m_finalBlitPass, "Final Blit");

        // When HDR is enabled, previous passes are rendering to the HDROutput buffer instead of backbuffer and we need to apply HDR curve when copying to backbuffer
        auto renderer = Renderer::get();
        if (gfx::HDR::None != renderer->GetHDR())
        {

        }

        _frameGraph.popPassGroup();
    }
}