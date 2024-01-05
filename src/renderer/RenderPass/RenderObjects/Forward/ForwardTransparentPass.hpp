#include "ForwardTransparentPass.h"
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
    ForwardTransparentPass::ForwardTransparentPass() :
        RenderObjectsPass("ForwardTransparentPass")
    {

    }

    //--------------------------------------------------------------------------------------
    ForwardTransparentPass::~ForwardTransparentPass()
    {

    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void ForwardTransparentPass::Setup(const gfx::RenderPassContext & _renderPassContext, float _dt)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));

        readRWBuffer("SkinningRWBuffer");

        View * view = (View *)_renderPassContext.m_view;
        readDepthStencil(view->getShadowMaps());
    }

    //--------------------------------------------------------------------------------------
    void ForwardTransparentPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_raytracing = view->IsUsingRayTracing();
        renderContext.m_shaderPass = ShaderPass::Transparent;

        const GraphicInstanceList & transparentInstances = view->getCullingJobResult().get(GraphicInstanceListType::Transparent);

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        const bool depthWrite = false;

        DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        DrawGraphicInstances(renderContext, _cmdList, transparentInstances);
    }
}