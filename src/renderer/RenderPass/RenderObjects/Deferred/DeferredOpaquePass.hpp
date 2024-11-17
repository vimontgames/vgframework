#include "DeferredOpaquePass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"
#include "renderer/View/Shadow/ShadowView.h"

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
    void DeferredOpaquePass::Setup(const gfx::RenderPassContext & _renderContext)
    {
        const auto size = _renderContext.getView()->GetSize();
        const auto options = RendererOptions::get();
        const auto msaa = options->GetMSAA();
        const TextureType texType = (MSAA::None == msaa) ? TextureType::Texture2D : TextureType::Texture2DMS;

        // Albedo
        {
            FrameGraphTextureResourceDesc albedoGBufferDesc;
                                          albedoGBufferDesc.type = texType;
                                          albedoGBufferDesc.msaa = msaa;
                                          albedoGBufferDesc.format = PixelFormat::R16G16B16A16_float;
                                          albedoGBufferDesc.width = size.x;
                                          albedoGBufferDesc.height = size.y;
                                          albedoGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          albedoGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto albedoGBufferID = _renderContext.getFrameGraphID("AlbedoGBuffer");
            createRenderTarget(albedoGBufferID, albedoGBufferDesc);
            writeRenderTarget(0, albedoGBufferID);
        }

        // Normal
        {
            FrameGraphTextureResourceDesc normalGBufferDesc;
                                          normalGBufferDesc.type = texType;
                                          normalGBufferDesc.msaa = msaa;
                                          normalGBufferDesc.format = PixelFormat::R16G16B16A16_float;
                                          normalGBufferDesc.width = size.x;
                                          normalGBufferDesc.height = size.y;
                                          normalGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          normalGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto normalGBufferID = _renderContext.getFrameGraphID("NormalGBuffer");
            createRenderTarget(normalGBufferID, normalGBufferDesc);
            writeRenderTarget(1, normalGBufferID);
        }

        // PBR
        {
            FrameGraphTextureResourceDesc pbrGBufferDesc;
                                          pbrGBufferDesc.type = texType;
                                          pbrGBufferDesc.msaa = msaa;
                                          pbrGBufferDesc.format = PixelFormat::R16G16B16A16_float;
                                          pbrGBufferDesc.width = size.x;
                                          pbrGBufferDesc.height = size.y;
                                          pbrGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          pbrGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto pbrGBufferID = _renderContext.getFrameGraphID("PBRGBuffer");
            createRenderTarget(pbrGBufferID, pbrGBufferDesc);
            writeRenderTarget(2, pbrGBufferID);
        }

        writeDepthStencil(_renderContext.getFrameGraphID("DepthStencil"));

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

        const bool depthWrite = options->isZPrepassEnabled() ? false : true;

        DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        DrawGraphicInstances(renderContext, _cmdList, GraphicInstanceListType::Opaque);
        DrawGraphicInstances(renderContext, _cmdList, GraphicInstanceListType::AlphaTest);
        DrawGraphicInstances(renderContext, _cmdList, GraphicInstanceListType::Decal);
    }
}