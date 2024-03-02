#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class EditorPass : public RenderObjectsPass
    {
    public:
        const char * GetClassName() const final { return "EditorPass"; }

        EditorPass();
        ~EditorPass();

        void    Setup       (const gfx::RenderPassContext & _renderPassContext) override;
        void	BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) override;
        void    Render      (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const override;
        void	AfterRender (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) override;

    private:
        gfx::Buffer * m_toolmodeRWBufferStaging = nullptr;
    };
}