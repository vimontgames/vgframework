#include "ForwardOpaquePass.h"
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
    ForwardOpaquePass::ForwardOpaquePass() :
        RenderObjectsPass("ForwardOpaquePass")
    {

    }

    //--------------------------------------------------------------------------------------
    ForwardOpaquePass::~ForwardOpaquePass()
    {
        
    }    

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void ForwardOpaquePass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        
        readRWBuffer("SkinningRWBuffer");

        const View * view = static_cast<const View*>(_renderPassContext.getView());
        readDepthStencil(view->getShadowMaps());
    }

    //--------------------------------------------------------------------------------------
    void ForwardOpaquePass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_raytracing = view->IsUsingRayTracing();
        renderContext.m_shaderPass = ShaderPass::Forward;

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
    }   
}