#include "EditorPass.h"
#include "core/GameObject/GameObject.h"
#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Model/Material/MaterialModel.h"
#include "renderer/View/View.h"
#include "renderer/Picking/PickingManager.h"
#include "renderer/DebugDraw/DebugDraw.h"
#include "renderer/Instance/GraphicInstance.h"

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
    void EditorPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        writeRenderTarget(0, _renderPassContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        writeRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));

        readRWBuffer("SkinningRWBuffer");
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        DebugDraw::get()->update(static_cast<const View*>(_renderPassContext.getView()), _cmdList);
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto options = RendererOptions::get();

        RenderContext renderContext;
        renderContext.m_renderPass = &_renderPassContext;
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

            // Default pass states
            RasterizerState rs(FillMode::Solid, CullMode::None, Orientation::ClockWise, DepthClip::Enable, DepthBias::None);
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
                DrawGraphicInstances(renderContext, _cmdList, GraphicInstanceListType::All);
            }

            bool boudingBoxSelection = true;
            if (options->isAABBEnabled() || boudingBoxSelection)
            {
                const GraphicInstanceList & allInstances = view->getCullingJobResult().get(GraphicInstanceListType::All);
                for (uint i = 0; i < allInstances.m_instances.size(); ++i)
                {
                    auto * instance = (GraphicInstance*)allInstances.m_instances[i];

                    if (boudingBoxSelection && !asBool(ObjectFlags::Selected & instance->getObjectFlags()))
                        continue;

                    AABB aabb;
                    if (instance->TryGetAABB(aabb))
                        dbgDraw->drawAABB(_cmdList, aabb, instance->getGlobalMatrix());
                }
            }
        }

        // Other Debug draw
        dbgDraw->render(static_cast<const View *>(_renderPassContext.getView()), _cmdList);
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::AfterRender(const RenderPassContext & _renderPassContext, CommandList * _cmdList)
    {
        auto * view = (IView*)(_renderPassContext.getViewMutable());
        Buffer * toolmodeRWBuffer = getRWBuffer(_renderPassContext.getFrameGraphID("ToolmodeRWBuffer"));

        // allocate staging copy
        BufferDesc stagingDesc = toolmodeRWBuffer->getBufDesc();
        stagingDesc.resource.m_usage = Usage::Staging;
        stagingDesc.resource.m_bindFlags = BindFlags::None;

        auto * device = gfx::Device::get();
        
        if (m_toolmodeRWBufferStaging == nullptr)
            m_toolmodeRWBufferStaging = device->createBuffer(stagingDesc, (toolmodeRWBuffer->GetName() + "_Staging").c_str());

        // copy to staging
        _cmdList->copyBuffer(m_toolmodeRWBufferStaging, toolmodeRWBuffer, 0);

        // TODO: wait or make sure the IDs remain valid even for deleted objects
        Map result = m_toolmodeRWBufferStaging->getResource().map();
        {
            const ToolmodeRWBufferData * data = (ToolmodeRWBufferData *)result.data;
            const PickingData * pickingData = &data->m_picking;
            view->SetPickingData(*pickingData);
        }
        m_toolmodeRWBufferStaging->getResource().unmap();
    }
}