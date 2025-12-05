#include "TLASUpdatePass.h"
#include "renderer/RayTracing/RayTracingManager.h"
#include "renderer/Options/RendererOptions.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    TLASUpdatePass::TLASUpdatePass() :
        UpdatePass("TLASUpdatePass")
    {

    }

    //--------------------------------------------------------------------------------------
    TLASUpdatePass::~TLASUpdatePass()
    {

    }

    //--------------------------------------------------------------------------------------
    // Main thread, before framegraph build
    //--------------------------------------------------------------------------------------
    void TLASUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {

    }

    //--------------------------------------------------------------------------------------
    // Main thread, before framegraph render
    //-------------------------------------------------------------------------------------
    void TLASUpdatePass::BeforeAll(const gfx::RenderPassContext & _renderPassContext)
    {
        RayTracingManager * rtManager = RayTracingManager::get();
        const auto & rendererOptions = RendererOptions::get();

        if (rendererOptions->getRayTracingTLASMode() == TLASMode::PerView)
            rtManager->prepareTLAS(static_cast<View *>(_renderPassContext.getViewMutable()));
        else
            rtManager->prepareTLAS(_renderPassContext.getWorld(), (core::vector<View*>&)_renderPassContext.getViews());
    }

    //--------------------------------------------------------------------------------------
    // Render job, before render pass (in this case there there is no draw/dipatch calls as
    // we only update buffers.
    //--------------------------------------------------------------------------------------
    void TLASUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        RayTracingManager * rtManager = RayTracingManager::get();

        const auto & rendererOptions = RendererOptions::get();

        if (rendererOptions->getRayTracingTLASMode() == TLASMode::PerView)
            rtManager->updateTLAS(_cmdList, static_cast<View*>(_renderPassContext.getViewMutable()));
        else
            rtManager->updateTLAS(_cmdList, _renderPassContext.getWorld(), (core::vector<View *>&)_renderPassContext.getViews());
    }
}
