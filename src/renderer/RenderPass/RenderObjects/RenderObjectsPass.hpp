#include "RenderObjectsPass.h"
#include "renderer/Job/Culling/GraphicInstanceList.h"
#include "renderer/Instance/GraphicInstance.h"
#include "renderer/Instance/Light/LightInstance.h"

#include "RenderObjects/DepthOnly/DepthOnlyPass.hpp"
#include "RenderObjects/DepthOnly/DepthPrepass/DepthPrePass.hpp"
#include "RenderObjects/DepthOnly/ShadowMap/ShadowMapPass.hpp"
#include "RenderObjects/Forward/ForwardOpaquePass.hpp"
#include "RenderObjects/Forward/ForwardTransparentPass.hpp"
#include "RenderObjects/Deferred/DeferredOpaquePass.hpp"
#include "RenderObjects/Editor/EditorPass.hpp"
#include "RenderObjects/Misc/Outline/OutlineMaskPass.hpp"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    RenderObjectsPass::RenderObjectsPass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Graphic);
    }

    //--------------------------------------------------------------------------------------
    RenderObjectsPass::~RenderObjectsPass()
    {

    }   

    //--------------------------------------------------------------------------------------
    core::u64 RenderObjectsPass::getListCostEstimate(const ViewCullingJobOutput & _cullingOutput, GraphicInstanceListType _list) const
    {
        return _cullingOutput.get(_list).m_instances.size();
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::DrawGraphicInstanceList(const RenderContext & _renderContext, gfx::CommandList * _cmdList, GraphicInstanceListType _list) const
    {
        const auto * view = static_cast<const View *>(_renderContext.m_renderPass->getView());
        const auto & list = view->getCullingJobResult().get(_list).m_instances;

        RenderContext renderContext = _renderContext;

        switch (_list)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_list);
                break;

            case GraphicInstanceListType::All:
            case GraphicInstanceListType::Outline:
            case GraphicInstanceListType::Opaque:
                renderContext.m_surfaceType = SurfaceType::Opaque;
                break;

            case GraphicInstanceListType::Transparent:
                renderContext.m_surfaceType = SurfaceType::AlphaBlend;
                break;

            case GraphicInstanceListType::AlphaTest:
                renderContext.m_surfaceType = SurfaceType::AlphaTest;
                break;

            case GraphicInstanceListType::Decal:
                renderContext.m_surfaceType = SurfaceType::Decal;
                break;
        }

        if (list.size() > 0)
        {
            VG_PROFILE_GPU(asCString(_list)); 

            for (uint i = 0; i < list.size(); ++i)
            {
                const GraphicInstance * instance = list[i];
                instance->Draw(renderContext, _cmdList);
            }
        }
    }  

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::DrawLightInstanceList(const RenderContext & _renderContext, gfx::CommandList * _cmdList, LightType _list) const
    {
        const auto * view = static_cast<const View *>(_renderContext.m_renderPass->getView());
        const auto & list = view->getCullingJobResult().get(_list).m_instances;

        RenderContext renderContext = _renderContext;

        if (list.size() > 0)
        {
            VG_PROFILE_GPU(asCString(_list));

            for (uint i = 0; i < list.size(); ++i)
            {
                const GraphicInstance * instance = list[i];
                instance->Draw(renderContext, _cmdList);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void RenderObjectsPass::DrawCameraInstanceList(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        const auto * view = static_cast<const View *>(_renderContext.m_renderPass->getView());
        const auto & list = view->getCullingJobResult().m_cameras;

        RenderContext renderContext = _renderContext;

        if (list.size() > 0)
        {
            VG_PROFILE_GPU("Cameras");

            for (uint i = 0; i < list.size(); ++i)
            {
                const GraphicInstance * instance = list[i];
                instance->Draw(renderContext, _cmdList);
            }
        }
    }    
}