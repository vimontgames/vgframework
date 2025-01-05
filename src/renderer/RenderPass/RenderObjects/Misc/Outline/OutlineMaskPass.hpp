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
        return getListCostEstimate(view->getCullingJobResult(), GraphicInstanceListType::Selected);
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
                                      outlineMaskResourceDesc.format = PixelFormat::R8G8B8A8_unorm;
                                      outlineMaskResourceDesc.clearColor = (float4)0.0f;

        const auto outlineMaskID = _renderPassContext.getFrameGraphID("OutlineMask");
        createRenderTarget(outlineMaskID, outlineMaskResourceDesc);
        writeRenderTarget(0, outlineMaskID);

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
        renderContext.m_shaderPass = ShaderPass::Transparent;

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        const bool depthWrite = false;

        DepthStencilState ds(false, false, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        DrawGraphicInstances(renderContext, _cmdList, GraphicInstanceListType::Selected);
    }
}