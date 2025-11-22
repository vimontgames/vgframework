#include "TLASUpdatePass.h"
#include "renderer/RayTracing/RayTracingManager.h"

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
        rtManager->prepareTLAS(static_cast<View *>(_renderPassContext.getViewMutable()));
    }

    //--------------------------------------------------------------------------------------
    // Render job, before render pass (in this case there there is no draw/dipatch calls as
    // we only update buffers.
    //--------------------------------------------------------------------------------------
    void TLASUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        RayTracingManager * rtManager = RayTracingManager::get();
        rtManager->updateTLAS(_cmdList, static_cast<View*>(_renderPassContext.getViewMutable()));
    }
}
