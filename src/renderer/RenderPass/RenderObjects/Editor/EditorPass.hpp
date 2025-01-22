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
#include "Shaders/system/editorpass.hlsli"

namespace vg::renderer
{
    //gfx::Buffer * EditorPass::s_editorPassConstantsBuffer = nullptr;

    //--------------------------------------------------------------------------------------
    EditorPass::EditorPass() :
        RenderObjectsPass("EditorPass")
    {
        auto * device = Device::get();

        //if (nullptr == s_editorPassConstantsBuffer)
        //{
        //    BufferDesc editorPassConstantsBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(EditorPassConstants));
        //    s_editorPassConstantsBuffer = device->createBuffer(editorPassConstantsBufferDesc, "EditorPassConstants", nullptr, ReservedSlot::EditorPassBufSrv);
        //}
        //else
        //{
        //    VG_SAFE_INCREASE_REFCOUNT(s_editorPassConstantsBuffer);
        //}
    }

    //--------------------------------------------------------------------------------------
    EditorPass::~EditorPass()
    {
        VG_SAFE_DELETE(m_toolmodeRWBufferStaging);

        //if (s_editorPassConstantsBuffer && !s_editorPassConstantsBuffer->Release())
        //    s_editorPassConstantsBuffer = nullptr;
    }

    //--------------------------------------------------------------------------------------
    core::u64 EditorPass::GetCostEstimate(const gfx::RenderPassContext & _renderPassContext) const 
    {
        const auto options = RendererOptions::get();
        const View * view = static_cast<const View *>(_renderPassContext.getView());
        const auto & cullingResult = view->getCullingJobResult();
        u64 cost = 2;

        if (options->isWireframeEnabled())
            cost += getListCostEstimate(cullingResult, GraphicInstanceListType::Opaque);

        if (options->isAABBEnabled())
            cost += getListCostEstimate(cullingResult, GraphicInstanceListType::Opaque);

        DebugDraw * dbgDraw = DebugDraw::get();
        cost += dbgDraw->getDebugDrawCount(view);

        return cost;
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

        //auto OutlineMaskTex = getRenderTarget(_renderPassContext.getFrameGraphID("OutlineMask"));
        //auto outlineMask = OutlineMaskTex->getTextureHandle();

        //EditorPassConstants * constants = (EditorPassConstants *)_cmdList->map(s_editorPassConstantsBuffer, sizeof(EditorPassConstants)).data;
        //{
        //    constants->setOutlineMask(outlineMask);
        //}
        //_cmdList->unmap(s_editorPassConstantsBuffer);
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

            if (options->isAABBEnabled())
            {
                const GraphicInstanceList & allInstances = view->getCullingJobResult().get(GraphicInstanceListType::All);
                for (uint i = 0; i < allInstances.m_instances.size(); ++i)
                {
                    auto * instance = (GraphicInstance*)allInstances.m_instances[i];

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
        const auto * view = (IView*)(_renderPassContext.getView());
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
            m_pickingData = *pickingData; ;
        }
        m_toolmodeRWBufferStaging->getResource().unmap();
    }

    //--------------------------------------------------------------------------------------
    void EditorPass::AfterAll(const gfx::RenderPassContext & _renderPassContext)
    {
        auto * view = (IView *)(_renderPassContext.getViewMutable());
        view->SetPickingData(m_pickingData);
    }
}