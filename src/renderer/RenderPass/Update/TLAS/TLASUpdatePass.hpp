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
    void TLASUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {

    }

    //--------------------------------------------------------------------------------------
    void TLASUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        RayTracingManager * rtManager = RayTracingManager::get();
        rtManager->updateView(_cmdList, (View*)_renderPassContext.m_view);
    }
}
