#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    class MeshModel;
    class AABB;

    //--------------------------------------------------------------------------------------
    class ForwardPass : public RenderObjectsPass
    {
    public:
        const char * getClassName() const final { return "ForwardPass"; }

        ForwardPass();
        ~ForwardPass();

        void Setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void Render(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:

    };
}