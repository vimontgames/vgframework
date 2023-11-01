#include "ForwardPass.h"

#include "core/GameObject/GameObject.h"

#include "gfx/RingBuffer/Dynamic/DynamicBuffer.h"

#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/View/View.h"

#include "gfx/RingBuffer/Upload/UploadBuffer.h"

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
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = DisplayOptions::get();

        RenderContext renderContext;
        renderContext.m_viewProj = view->getViewProjMatrix();
        renderContext.m_toolmode = view->getViewID().target == gfx::ViewTarget::Editor || options->isToolModeEnabled();

        const GraphicInstanceList & allInstances = view->m_cullingJobResult.m_instanceLists[asInteger(GraphicInstanceListType::All)];
        
        // Default pass states
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(true, true, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        if (!renderContext.m_toolmode || options->isOpaqueEnabled())
            DrawGraphicInstances(renderContext, _cmdList, allInstances);

        if (renderContext.m_toolmode)
        {
            if (options->isWireframeEnabled())
            {
                renderContext.m_wireframe = true;
                DrawGraphicInstances(renderContext, _cmdList, allInstances);
            }
        }

        if (renderContext.m_toolmode)
        {
            if (options->isAABBEnabled())
            {
                for (uint i = 0; i < allInstances.m_instances.size(); ++i)
                {
                    const IGraphicInstance * instance = allInstances.m_instances[i];
                    const MeshModel * model = (MeshModel *)instance->getModel(Lod::Lod0);
                    if (nullptr == model)
                        continue;

                    const MeshGeometry * geo = model->getGeometry();
                    drawAABB(_cmdList, geo->getAABB(), instance->getWorldMatrix(), renderContext.m_viewProj);
                }
            }

            drawGrid(_cmdList, renderContext.m_viewProj);
            drawAxis(_cmdList, renderContext.m_viewProj);
        }
    }   
}