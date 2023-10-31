#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::gfx
{
    class Buffer;
    class DynamicBuffer;
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

        void setup(const gfx::RenderPassContext & _renderContext, double _dt) override;
        void draw(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:

        //gfx::DynamicBuffer * m_dynamicBuffer = nullptr;
        //gfx::Buffer * m_updatedBuffer = nullptr;

        gfx::Buffer * m_constantBuffer = nullptr;
    };
}