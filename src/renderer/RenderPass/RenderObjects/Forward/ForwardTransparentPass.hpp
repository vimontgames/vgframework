#include "ForwardTransparentPass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"
#include "renderer/RenderPass/Update/ViewConstants/ViewConstantsUpdatePass.h"
#include "Shaders/system/transparentPass.hlsli"

namespace vg::renderer
{
    gfx::Buffer * ForwardTransparentPass::s_transparentPassConstantsBuffer = nullptr;

    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    ForwardTransparentPass::ForwardTransparentPass() :
        RenderObjectsPass("ForwardTransparentPass")
    {
        auto * device = Device::get();

        if (nullptr == s_transparentPassConstantsBuffer)
        {
            BufferDesc editorPassConstantsBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(TransparentPassConstants));
            s_transparentPassConstantsBuffer = device->createBuffer(editorPassConstantsBufferDesc, "TransparentPassConstants", nullptr, ReservedSlot::TransparentPassBufSrv);
        }
        else
        {
            VG_SAFE_INCREASE_REFCOUNT(s_transparentPassConstantsBuffer);
        }
    }

    //--------------------------------------------------------------------------------------
    ForwardTransparentPass::~ForwardTransparentPass()
    {
        if (s_transparentPassConstantsBuffer && !s_transparentPassConstantsBuffer->Release())
            s_transparentPassConstantsBuffer = nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::u64 ForwardTransparentPass::GetCostEstimate(const RenderPassContext & _renderPassContext) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        return getListCostEstimate(view->getCullingJobResult(), GraphicInstanceListType::Transparent);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void ForwardTransparentPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));

        readRWBuffer("SkinningRWBuffer");

        const View * view = static_cast<const View *>(_renderPassContext.getView());
        readDepthStencil(view->getShadowMaps());

        const auto linearDepthID = _renderPassContext.getFrameGraphID("LinearDepth");
        readRenderTarget(linearDepthID);
    }

    //--------------------------------------------------------------------------------------
    void ForwardTransparentPass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        auto linearDepthTex = getRenderTarget(_renderPassContext.getFrameGraphID("LinearDepth"));
        auto linearDepth = linearDepthTex->getTextureHandle();

        TransparentPassConstants * constants = (TransparentPassConstants *)_cmdList->map(s_transparentPassConstantsBuffer, sizeof(TransparentPassConstants)).data;
        {
            constants->setLinearDepth(linearDepth);
        }
        _cmdList->unmap(s_transparentPassConstantsBuffer);
    }

    //--------------------------------------------------------------------------------------
    void ForwardTransparentPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
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

        DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::Transparent);
    }
}