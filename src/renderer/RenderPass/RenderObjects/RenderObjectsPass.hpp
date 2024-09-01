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
    void RenderObjectsPass::DrawGraphicInstances(const RenderContext & _renderContext, CommandList * _cmdList, const GraphicInstanceList & _graphicInstancesList) const
    {
        const auto & list = _graphicInstancesList.m_instances;
        bool wireframeSelection = false; // TODO: expose option for selection

        for (uint i = 0; i < list.size(); ++i)
        {
            const GraphicInstance * instance = list[i];

            if (_renderContext.m_wireframe && wireframeSelection)
            {
                if (!asBool(ObjectFlags::Selected & instance->getObjectFlags()))
                    continue;
            }

            instance->Draw(_renderContext, _cmdList);
        }
    }
}