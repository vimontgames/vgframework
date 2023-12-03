#include "RenderObjectsPass.h"
#include "renderer/Job/Culling/GraphicInstanceList.h"
#include "renderer/IGraphicInstance.h"

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
            const IGraphicInstance * instance = list[i];

            if (_renderContext.m_wireframe && wireframeSelection)
            {
                if (!asBool(IInstance::RuntimeFlags::Selected & instance->getRuntimeFlags()))
                    continue;
            }

            instance->Draw(_renderContext, _cmdList);
        }
    }
}