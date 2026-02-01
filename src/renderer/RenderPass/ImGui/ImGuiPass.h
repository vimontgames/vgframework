#pragma once

#include "gfx/FrameGraph/UserPass.h"

namespace vg::engine
{
    class IEngine;
}

namespace vg::renderer
{
    class ImguiEditor;
    class ImGuiToolbar;

    //--------------------------------------------------------------------------------------
    class ImGuiPass final : public gfx::UserPass
    {
    public:
        const char * GetClassName() const final { return "ImGuiPass"; }

        ImGuiPass();
        ~ImGuiPass();

        core::u64   GetCostEstimate (const gfx::RenderPassContext & _renderPassContext) const final override;
        void        Setup           (const gfx::RenderPassContext & _renderContext) final override;
        void        Render          (const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const final override;

    private:
        static const vg::engine::IEngine * getEngine();
    };
}