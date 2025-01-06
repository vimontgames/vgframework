#pragma once

#include "renderer/RenderPass/RenderObjects/RenderObjectsPass.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    class EditorPass final : public RenderObjectsPass
    {
    public:
        const char * GetClassName() const final { return "EditorPass"; }

        EditorPass();
        ~EditorPass();

        void    Setup       (const gfx::RenderPassContext & _renderPassContext) final override;
        void	BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;
        void    Render      (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const final override;
        void	AfterRender (const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) final override;
        void	AfterAll    (const gfx::RenderPassContext & _renderPassContext) final override;

    private:
        //static gfx::Buffer * s_editorPassConstantsBuffer;

        gfx::Buffer * m_toolmodeRWBufferStaging = nullptr;
        PickingData m_pickingData;
    };
}