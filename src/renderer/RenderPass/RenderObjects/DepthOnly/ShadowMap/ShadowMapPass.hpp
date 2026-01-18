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
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_renderPass = &_renderPassContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_shaderPass = ShaderPass::ZOnly;
        renderContext.m_reverseCullMode = true; // Reverse cull Only for directional?

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        // Render full opaque then alphatest
        DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::Opaque);
        DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::AlphaTest);
    }
}