#include "DeferredOpaquePass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"
#include "renderer/View/Shadow/ShadowView.h"

#include "Shaders/lighting/gbuffer.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    DeferredOpaquePass::DeferredOpaquePass() :
        RenderObjectsPass("DeferredOpaquePass")
    {

    }

    //--------------------------------------------------------------------------------------
    DeferredOpaquePass::~DeferredOpaquePass()
    {

    }

    //--------------------------------------------------------------------------------------
    core::u64 DeferredOpaquePass::GetCostEstimate(const RenderPassContext & _renderPassContext) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto & cullingResult = view->getCullingJobResult();
        return getListCostEstimate(cullingResult, GraphicInstanceListType::Opaque)
             + getListCostEstimate(cullingResult, GraphicInstanceListType::AlphaTest)
             + getListCostEstimate(cullingResult, GraphicInstanceListType::Decal);
    }

    //--------------------------------------------------------------------------------------
    void DeferredOpaquePass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto * view = (IView *)_renderPassContext.getView();
        const auto size = view->GetSize();
        const auto options = RendererOptions::get();
        const auto msaa = options->GetMSAA();
        const TextureType texType = (MSAA::None == msaa) ? TextureType::Texture2D : TextureType::Texture2DMS;

        auto * device = gfx::Device::get();
        auto * renderer = Renderer::get();

        // Albedo
        {
            FrameGraphTextureResourceDesc albedoGBufferDesc;
                                          albedoGBufferDesc.type = texType;
                                          albedoGBufferDesc.msaa = msaa;
                                          albedoGBufferDesc.format = renderer->getGBufferFormat(GBuffer::Albedo);
                                          albedoGBufferDesc.width = size.x;
                                          albedoGBufferDesc.height = size.y;
                                          albedoGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          albedoGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto albedoGBufferID = _renderPassContext.getFrameGraphID("AlbedoGBuffer");
            createRenderTarget(albedoGBufferID, albedoGBufferDesc);
            writeRenderTarget(0, albedoGBufferID);
        }

        // Normal
        {
            FrameGraphTextureResourceDesc normalGBufferDesc;
                                          normalGBufferDesc.type = texType;
                                          normalGBufferDesc.msaa = msaa;
                                          normalGBufferDesc.format = renderer->getGBufferFormat(GBuffer::Normal);
                                          normalGBufferDesc.width = size.x;
                                          normalGBufferDesc.height = size.y;
                                          normalGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          normalGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto normalGBufferID = _renderPassContext.getFrameGraphID("NormalGBuffer");
            createRenderTarget(normalGBufferID, normalGBufferDesc);
            writeRenderTarget(1, normalGBufferID);
        }

        // PBR
        {
            FrameGraphTextureResourceDesc pbrGBufferDesc;
                                          pbrGBufferDesc.type = texType;
                                          pbrGBufferDesc.msaa = msaa;
                                          pbrGBufferDesc.format = renderer->getGBufferFormat(GBuffer::PBR);
                                          pbrGBufferDesc.width = size.x;
                                          pbrGBufferDesc.height = size.y;
                                          pbrGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          pbrGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto pbrGBufferID = _renderPassContext.getFrameGraphID("PBRGBuffer");
            createRenderTarget(pbrGBufferID, pbrGBufferDesc);
            writeRenderTarget(2, pbrGBufferID);
        }

        // Emissive
        {
            FrameGraphTextureResourceDesc emissiveGBufferDesc;
            emissiveGBufferDesc.type = texType;
            emissiveGBufferDesc.msaa = msaa;
            emissiveGBufferDesc.format = renderer->getGBufferFormat(GBuffer::Emissive);
            emissiveGBufferDesc.width = size.x;
            emissiveGBufferDesc.height = size.y;
            emissiveGBufferDesc.clearColor = defaultOptimizedClearColor;
            emissiveGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto emissiveGBufferID = _renderPassContext.getFrameGraphID("EmissiveGBuffer");
            createRenderTarget(emissiveGBufferID, emissiveGBufferDesc);
            writeRenderTarget(3, emissiveGBufferID);
        }

        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));

        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void DeferredOpaquePass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_renderPass = &_renderPassContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_shaderPass = ShaderPass::Deferred;

        // Default pass states
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);

        // Enable depth writes for opaque if no ZPrepass
        {
            const bool depthWrite = options->isZPrepassEnabled() ? false : true;
            DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);
            _cmdList->setDepthStencilState(ds);
            DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::Opaque);
            DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::AlphaTest);
        }

        // Always disable depth writes when rendering decals
        {
            DepthStencilState ds(true, false, ComparisonFunc::LessEqual);
            _cmdList->setDepthStencilState(ds);
            DrawGraphicInstanceList(renderContext, _cmdList, GraphicInstanceListType::Decal);
        }
    }
}