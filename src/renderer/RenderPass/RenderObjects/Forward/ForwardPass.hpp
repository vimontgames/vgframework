#include "ForwardPass.h"

#include "core/GameObject/GameObject.h"

#include "gfx/RingBuffer/Dynamic/DynamicBuffer.h"

#include "renderer/Geometry/Mesh/MeshGeometry.h"
#include "renderer/Model/Mesh/MeshModel.h"
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
        //VG_SAFE_RELEASE(m_dynamicBuffer);
        //VG_SAFE_RELEASE(m_updatedBuffer);
    }    

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void ForwardPass::setup(const gfx::RenderPassContext & _renderContext, double _dt)
    {
        writeRenderTarget(0, _renderContext.getFrameGraphID("Color"));
        writeDepthStencil(_renderContext.getFrameGraphID("DepthStencil"));

        // test (Can't 'copyBuffer' during RenderPass)
        //u8 * data = m_dynamicBuffer->map(sizeof(float4), 256);
        //{
        //    float4 color = float4(0, 1, 0, 1);
        //    memcpy(data, &color, sizeof(float4));
        //}
        //m_dynamicBuffer->unmap();
        //
        //// kwik Haxx
        //auto * device = Device::get();
        //CommandList * _cmdList = device->getCommandLists(CommandListType::Graphics)[0];
        //
        //// Probably need some syntaxic sugar here
        //_cmdList->copyBuffer(m_updatedBuffer, m_dynamicBuffer->getBuffer(), data - m_dynamicBuffer->getBaseAddress());
    }

    //--------------------------------------------------------------------------------------
    void ForwardPass::draw(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
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

        DrawGraphicInstances(renderContext, _cmdList, allInstances);

        if (renderContext.m_toolmode)
        {
            if (options->isWireframeEnabled())
            {
                renderContext.m_wireframe = true;
                DrawGraphicInstances(renderContext, _cmdList, allInstances);
            }
        }

        // TODO: create base class "GraphicInstance" for common implementation between all types of instances?
        // e.g. ConfigureToolmode(&flags, &mode) 
        //u16 flags = 0x0, mode = 0x0;
        //
        //if (toolmode)
        //{
        //
        //    if (options->isAlbedoMapsEnabled())
        //        flags |= FLAG_ALBEDOMAPS;
        //
        //    if (options->isNormalMapsEnabled())
        //        flags |= FLAG_NORMALMAPS;
        //
        //    switch (options->getDisplayMode())
        //    {
        //        default:
        //            VG_ASSERT_ENUM_NOT_IMPLEMENTED(options->getDisplayMode());
        //            break;
        //
        //        case DisplayMode::Default:
        //            mode = MODE_DEFAULT;
        //            break;
        //
        //        case DisplayMode::MatID:
        //            mode = MODE_MATID;
        //            break;
        //
        //        case DisplayMode::VSNormal:
        //            mode = MODE_VS_NORMAL;
        //            break;
        //
        //        case DisplayMode::VSTangent:
        //            mode = MODE_VS_TANGENT;
        //            break;
        //
        //        case DisplayMode::VSBinormal:
        //            mode = MODE_VS_BINORMAL;
        //            break;
        //
        //        case DisplayMode::VSColor:
        //            mode = MODE_VS_COLOR;
        //            break;
        //
        //        case DisplayMode::UV0:
        //            mode = MODE_UV0;
        //            break;
        //
        //        case DisplayMode::UV1:
        //            mode = MODE_UV1;
        //            break;
        //
        //        case DisplayMode::Albedo:
        //            mode = MODE_ALBEDO_MAP;
        //            break;
        //
        //        case DisplayMode::PSNormal:
        //            mode = MODE_NORMAL_MAP;
        //            break;
        //    }
        //}

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