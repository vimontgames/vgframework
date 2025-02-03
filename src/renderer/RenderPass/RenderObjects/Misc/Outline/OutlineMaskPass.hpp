#include "OutlineMaskPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    OutlineMaskPass::OutlineMaskPass() :
        RenderObjectsPass("OutlineMaskPass")
    {

    }

    //--------------------------------------------------------------------------------------
    OutlineMaskPass::~OutlineMaskPass()
    {

    }

    //--------------------------------------------------------------------------------------
    core::u64 OutlineMaskPass::GetCostEstimate(const RenderPassContext & _renderPassContext) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        return getListCostEstimate(view->getCullingJobResult(), GraphicInstanceListType::Outline);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void OutlineMaskPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        // Create outline mask texture
        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");

        const FrameGraphTextureResourceDesc * depthStencilResourceDesc = getTextureResourceDesc(depthStencilID);

        FrameGraphTextureResourceDesc outlineMaskResourceDesc = *depthStencilResourceDesc;
                                      outlineMaskResourceDesc.type = TextureType::Texture2D;
                                      outlineMaskResourceDesc.msaa = MSAA::None;
                                      outlineMaskResourceDesc.format = PixelFormat::R16G16B16A16_uint;  // up to 4 outline masks, alpha channel used for editor selection outline
                                      outlineMaskResourceDesc.clearColor = (float4)0.0f;

        const auto outlineMaskID = _renderPassContext.getFrameGraphID("OutlineMask");
        createRenderTarget(outlineMaskID, outlineMaskResourceDesc);
        writeRenderTarget(0, outlineMaskID);

        FrameGraphTextureResourceDesc outlineMaskDepthStencilResourceDesc = *depthStencilResourceDesc;
                                      outlineMaskDepthStencilResourceDesc.type = TextureType::Texture2D;
                                      outlineMaskDepthStencilResourceDesc.msaa = MSAA::None;

        const auto outlineMaskDepthStencilID = _renderPassContext.getFrameGraphID("OutlineMaskDepthStencil");
        createRenderTarget(outlineMaskDepthStencilID, outlineMaskDepthStencilResourceDesc);
        writeDepthStencil(outlineMaskDepthStencilID);

        const auto linearDepthID = _renderPassContext.getFrameGraphID("LinearDepth");
        readRenderTarget(linearDepthID);

        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void OutlineMaskPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_renderPass = &_renderPassContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_raytracing = view->IsUsingRayTracing();
        renderContext.m_shaderPass = ShaderPass::Outline;
        renderContext.m_outline = true;

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::Outline);
    }
}