#include "DepthPrePass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    DepthPrePass::DepthPrePass() :
        DepthOnlyPass("DepthPrepass")
    {

    }

    //--------------------------------------------------------------------------------------
    DepthPrePass::~DepthPrePass()
    {

    }

    //--------------------------------------------------------------------------------------
    void DepthPrePass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));

        readRWBuffer("SkinningRWBuffer");
    }   
}