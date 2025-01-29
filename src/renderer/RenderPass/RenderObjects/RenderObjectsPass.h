#pragma once

#include "renderer/RenderPass/RenderPass.h"
#include "renderer/RenderPass/RenderContext.h"

namespace vg::renderer
{
    class RenderContext;
    class GraphicInstanceList;

    struct ViewCullingJobOutput;

    enum class GraphicInstanceListType : core::u8;
    enum class LightType : core::u8;

    //--------------------------------------------------------------------------------------
    class RenderObjectsPass : public RenderPass
    {
    public:
        VG_CLASS_DECL(RenderObjectsPass, RenderPass);

        RenderObjectsPass(const core::string & _name);
        ~RenderObjectsPass();

    protected:
        virtual void    DrawGraphicInstances(const RenderContext & _renderContext, gfx::CommandList * _cmdList, GraphicInstanceListType _list) const;
        virtual void    DrawGraphicInstances(const RenderContext & _renderContext, gfx::CommandList * _cmdList, LightType _list) const;
        core::u64       getListCostEstimate(const ViewCullingJobOutput & _cullingOutput, GraphicInstanceListType _list) const;
    };
}