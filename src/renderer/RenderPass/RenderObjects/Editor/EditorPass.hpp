#include "EditorPass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    EditorPass::EditorPass() :
        RenderObjectsPass("EditorPass")
    {

    }

    //--------------------------------------------------------------------------------------
    EditorPass::~EditorPass()
    {

    }

    //--------------------------------------------------------------------------------------
    void EditorPass::Setup(const gfx::RenderPassContext & _renderPassContext, double _dt)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        //writeRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = DisplayOptions::get();

        RenderContext renderContext;
        renderContext.m_viewProj = view->getViewProjMatrix();
        renderContext.m_toolmode = view->isToolmode();
        renderContext.m_shaderPass = ShaderPass::Forward;

        bool opaque = true;

        const GraphicInstanceList & allInstances = view->m_cullingJobResult.m_instanceLists[asInteger(GraphicInstanceListType::All)];

        // Default pass states
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        const bool depthWrite = opaque && options->isZPrepassEnabled() ? false : true;

        DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        if (options->isWireframeEnabled())
        {
            renderContext.m_wireframe = true;
            DrawGraphicInstances(renderContext, _cmdList, allInstances);
        }

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

        //Buffer * toolmodeRWBuffer = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));
        //
        //// allocate staging copy
        //BufferDesc stagingDesc = toolmodeRWBuffer->getBufDesc();
        //stagingDesc.resource.m_usage = Usage::Staging;
        //stagingDesc.resource.m_bindFlags = BindFlags::None;
        //
        //auto * device = gfx::Device::get();
        //Buffer * stagingBuffer = device->createBuffer(stagingDesc, (toolmodeRWBuffer->getName() + "_Staging").c_str());
        //Map result = stagingBuffer->getResource().map();
        //{
        //
        //}
        //stagingBuffer->getResource().unmap();
        //
        //VG_SAFE_DELETE(stagingBuffer);
    }
}