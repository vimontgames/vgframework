#include "EditorPass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"
#include "renderer/Picking/PickingManager.h"
#include "renderer/DebugDraw/DebugDraw.h"

#include "Shaders/system/toolmode.hlsl.h"

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
        VG_SAFE_DELETE(m_toolmodeRWBufferStaging);
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::Setup(const gfx::RenderPassContext & _renderPassContext, float _dt)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        writeRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));

        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        DebugDraw::get()->update((View*)_renderPassContext.m_view, _cmdList);
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = (const View *)_renderPassContext.m_view;
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_view = view->getViewMatrix();
        renderContext.m_proj = view->getProjMatrix();
        renderContext.m_toolmode = view->isToolmode();
        renderContext.m_shaderPass = ShaderPass::Forward;

        DebugDraw * dbgDraw = DebugDraw::get();

        // Draw grid & axis
        dbgDraw->drawGrid(_cmdList);
        dbgDraw->drawAxis(_cmdList);

        // Draw AABBs
        {
            bool opaque = true;

            const GraphicInstanceList & allInstances = view->m_cullingJobResult.m_instanceLists[asInteger(GraphicInstanceListType::All)];

            // Default pass states
            RasterizerState rs(FillMode::Solid, CullMode::None, Orientation::ClockWise, DepthClip::Enable, DepthBias::None);
            BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

            const bool depthWrite = opaque && options->isZPrepassEnabled() ? false : true;

            DepthStencilState ds(true, depthWrite, ComparisonFunc::LessEqual);

            _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleList);
            _cmdList->setRasterizerState(rs);
            _cmdList->setBlendState(bs);
            _cmdList->setDepthStencilState(ds);

            bool wireframeSelection = false; // TODO: expose option for selection
            if (options->isWireframeEnabled() || wireframeSelection)
            {
                renderContext.m_wireframe = true;
                DrawGraphicInstances(renderContext, _cmdList, allInstances);
            }

            bool boudingBoxSelection = true;
            if (options->isAABBEnabled() || boudingBoxSelection)
            {
                for (uint i = 0; i < allInstances.m_instances.size(); ++i)
                {
                    const IGraphicInstance * instance = allInstances.m_instances[i];

                    if (boudingBoxSelection && !asBool(IInstance::RuntimeFlags::Selected & instance->getRuntimeFlags()))
                        continue;

                    const MeshModel * model = (MeshModel *)instance->getModel(Lod::Lod0); // TODO: get LoD from culling result
                    if (nullptr != model)
                    {
                        const MeshGeometry * geo = model->getGeometry();
                        const AABB & aabb = geo->getAABB();
                        dbgDraw->drawAABB(_cmdList, geo->getAABB(), instance->getWorldMatrix());
                        //dbgDraw->AddWireframeBox(aabb.m_min, aabb.m_max, 0xFF00FF00, instance->getWorldMatrix());
                    }
                }
            }
        }

        // Other Debug draw
        dbgDraw->render((View *)_renderPassContext.m_view, _cmdList);
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::AfterRender(const RenderPassContext & _renderPassContext, CommandList * _cmdList)
    {
        Buffer * toolmodeRWBuffer = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));

        // allocate staging copy
        BufferDesc stagingDesc = toolmodeRWBuffer->getBufDesc();
        stagingDesc.resource.m_usage = Usage::Staging;
        stagingDesc.resource.m_bindFlags = BindFlags::None;

        auto * device = gfx::Device::get();
        
        if (m_toolmodeRWBufferStaging == nullptr)
            m_toolmodeRWBufferStaging = device->createBuffer(stagingDesc, (toolmodeRWBuffer->getName() + "_Staging").c_str());

        // copy to staging
        _cmdList->copyBuffer(m_toolmodeRWBufferStaging, toolmodeRWBuffer);

        // TODO: wait or make sure the IDs remain valid even for deleted objects
        Map result = m_toolmodeRWBufferStaging->getResource().map();
        {
            const ToolmodeRWBufferData * data = (ToolmodeRWBufferData *)result.data;
            const PickingData * pickingData = &data->m_picking;
            _renderPassContext.m_view->SetPickingData(*pickingData);
        }
        m_toolmodeRWBufferStaging->getResource().unmap();
    }
}