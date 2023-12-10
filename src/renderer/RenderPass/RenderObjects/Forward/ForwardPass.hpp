#include "ForwardPass.h"
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
    ForwardPass::ForwardPass() :
        RenderObjectsPass("ForwardPass")
    {

    }

    //--------------------------------------------------------------------------------------
    ForwardPass::~ForwardPass()
    {
        
    }    

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void ForwardPass::Setup(const gfx::RenderPassContext & _renderContext, double _dt)
    {
        writeRenderTarget(0, _renderContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderContext.getFrameGraphID("DepthStencil"));
        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewInvMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_shaderPass = ShaderPass::Forward;

        // TODO: transparent forward pass
        bool opaque = true;

        const GraphicInstanceList & allInstances = view->m_cullingJobResult.m_instanceLists[asInteger(opaque ? GraphicInstanceListType::Opaque : GraphicInstanceListType::Transparent)];
        
        // Default pass states
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        
        const bool depthWrite = opaque && options->isZPrepassEnabled() ? false : true;

        DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        if (!renderContext.m_toolmode || options->isOpaqueEnabled())
            DrawGraphicInstances(renderContext, _cmdList, allInstances);
    }   
}