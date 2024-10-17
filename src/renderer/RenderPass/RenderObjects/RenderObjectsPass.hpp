#include "RenderObjectsPass.h"
#include "renderer/Job/Culling/GraphicInstanceList.h"
#include "renderer/Instance/GraphicInstance.h"

#include "RenderObjects/DepthOnly/DepthOnlyPass.hpp"
#include "RenderObjects/DepthOnly/DepthPrepass/DepthPrePass.hpp"
#include "RenderObjects/DepthOnly/ShadowMap/ShadowMapPass.hpp"
#include "RenderObjects/Forward/ForwardOpaquePass.hpp"
#include "RenderObjects/Forward/ForwardTransparentPass.hpp"
#include "RenderObjects/Deferred/DeferredOpaquePass.hpp"
#include "RenderObjects/Editor/EditorPass.hpp"

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
    void RenderObjectsPass::DrawGraphicInstances(const RenderContext & _renderContext, gfx::CommandList * _cmdList, GraphicInstanceListType _list) const
    {
        const auto * view = static_cast<const View *>(_renderContext.m_renderPass->getView());
        const auto & list = view->getCullingJobResult().get(_list).m_instances;

        bool wireframeSelection = false; // TODO: expose option for selection

        RenderContext renderContext = _renderContext;

        switch (_list)
        {
            default:
                break;

            case GraphicInstanceListType::AlphaTest:
                renderContext.m_alphatest = true;
                break;
        }

        for (uint i = 0; i < list.size(); ++i)
        {
            const GraphicInstance * instance = list[i];

            if (_renderContext.m_wireframe && wireframeSelection)
            {
                if (!asBool(ObjectFlags::Selected & instance->getObjectFlags()))
                    continue;
            }

            instance->Draw(renderContext, _cmdList);
        }
    }    
}