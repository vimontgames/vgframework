#pragma once

#include "renderer/RenderPass/RenderPass.h"
#include "renderer/RenderPass/RenderContext.h"

namespace vg::renderer
{
    class RenderContext;
    class GraphicInstanceList;

    //--------------------------------------------------------------------------------------
    class RenderObjectsPass : public RenderPass
    {
    public:
        RenderObjectsPass(const core::string & _name);
        ~RenderObjectsPass();

    protected:
        virtual void    DrawGraphicInstances    (const RenderContext & _renderContext, gfx::CommandList * _cmdList, const GraphicInstanceList & _graphicInstancesList) const;

    };
}