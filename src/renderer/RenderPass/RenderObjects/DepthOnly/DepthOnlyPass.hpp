#include "DepthOnlyPass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    DepthOnlyPass::DepthOnlyPass(const core::string & _name) :
        RenderObjectsPass(_name)
    {

    }

    //--------------------------------------------------------------------------------------
    DepthOnlyPass::~DepthOnlyPass()
    {

    }

    //--------------------------------------------------------------------------------------
    void DepthOnlyPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();
        renderContext.m_shaderPass = ShaderPass::ZOnly;

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        // Render full opaque then alphatest
        const GraphicInstanceListType opaqueLists[] =
        {
            GraphicInstanceListType::Opaque,
            GraphicInstanceListType::AlphaTest
        };

        for (uint i = 0; i < countof(opaqueLists); ++i)
        {
            const auto list = (GraphicInstanceListType)opaqueLists[i];
            const GraphicInstanceList & instances = view->getCullingJobResult().get(list);
            renderContext.m_alphatest = (GraphicInstanceListType::AlphaTest == list) ? true : false;
            DrawGraphicInstances(renderContext, _cmdList, instances);
        }
    }
}