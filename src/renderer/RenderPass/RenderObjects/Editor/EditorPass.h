#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class EditorPass : public RenderObjectsPass
    {
    public:
        const char * getClassName() const final { return "EditorPass"; }

        EditorPass();
        ~EditorPass();

        void    Setup       (const gfx::RenderPassContext & _renderPassContext, double _dt) override;
        void    Render      (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
        void	AfterRender (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) override;

    private:
        gfx::Buffer * m_toolmodeRWBufferStaging = nullptr;
    };
}