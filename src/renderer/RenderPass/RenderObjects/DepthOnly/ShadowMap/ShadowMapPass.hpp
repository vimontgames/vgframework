#include "ShadowMapPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ShadowMapPass::ShadowMapPass() :
        DepthOnlyPass("ShadowMapPass")
    {
        VG_PROFILE_CPU("New ShadowMapPass");
    }

    //--------------------------------------------------------------------------------------
    ShadowMapPass::~ShadowMapPass()
    {

    }

    //--------------------------------------------------------------------------------------
    void ShadowMapPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto * view = (IView *)_renderPassContext.getView();
        const auto size = view->GetSize();

        FrameGraphTextureResourceDesc shadowMapDesc = {};
        shadowMapDesc.format = PixelFormat::D32S8;
        shadowMapDesc.width = size.x;
        shadowMapDesc.height = size.y;
        shadowMapDesc.clearDepth = defaultOptimizedClearDepth;
        shadowMapDesc.clearStencil = defaultOptimizedClearStencil;
        shadowMapDesc.initState = FrameGraphResource::InitState::Clear;

        const auto shadowMapID = _renderPassContext.getFrameGraphIDEx("ShadowMap");
        createDepthStencil(shadowMapID, shadowMapDesc);
        writeDepthStencil(shadowMapID);
    }

    //--------------------------------------------------------------------------------------
    void ShadowMapPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        super::Render(_renderPassContext, _cmdList);
    }
}