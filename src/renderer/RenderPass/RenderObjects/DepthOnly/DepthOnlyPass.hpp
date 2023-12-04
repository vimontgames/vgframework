#include "DepthOnlyPass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    DepthOnlyPass::DepthOnlyPass() :
        RenderObjectsPass("DepthOnlyPass")
    {

    }

    //--------------------------------------------------------------------------------------
    DepthOnlyPass::~DepthOnlyPass()
    {

    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void DepthOnlyPass::Setup(const gfx::RenderPassContext & _renderPassContext, double _dt)
    {
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void DepthOnlyPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = DisplayOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewInvMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_shaderPass = ShaderPass::ZOnly;

        const GraphicInstanceList & allInstances = view->m_cullingJobResult.m_instanceLists[asInteger(GraphicInstanceListType::Opaque)];

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        if (!renderContext.m_toolmode || options->isOpaqueEnabled())
            DrawGraphicInstances(renderContext, _cmdList, allInstances);        
    }
}