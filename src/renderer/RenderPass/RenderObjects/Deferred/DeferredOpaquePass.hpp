#include "DeferredOpaquePass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"

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
    void DeferredOpaquePass::Setup(const gfx::RenderPassContext & _renderContext, float _dt)
    {
        auto size = _renderContext.m_view->GetSize();

        // Albedo
        {
            FrameGraphTextureResourceDesc albedoGBufferDesc;
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
                                          normalGBufferDesc.format = PixelFormat::R16G16B16A16_float;
                                          normalGBufferDesc.width = size.x;
                                          normalGBufferDesc.height = size.y;
                                          normalGBufferDesc.clearColor = defaultOptimizedClearColor;
                                          normalGBufferDesc.initState = FrameGraphResource::InitState::Clear;

            const auto normalGBufferID = _renderContext.getFrameGraphID("NormalGBuffer");
            createRenderTarget(normalGBufferID, normalGBufferDesc);
            writeRenderTarget(1, normalGBufferID);
        }

        writeDepthStencil(_renderContext.getFrameGraphID("DepthStencil"));

        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void DeferredOpaquePass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_shaderPass = ShaderPass::Deferred;

        const GraphicInstanceList & allInstances = view->m_cullingJobResult.m_instanceLists[asInteger(GraphicInstanceListType::Opaque)];

        // Default pass states
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        const bool depthWrite = options->isZPrepassEnabled() ? false : true;

        DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        DrawGraphicInstances(renderContext, _cmdList, allInstances);
    }
}