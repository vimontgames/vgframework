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
    class ImGuiPass : public gfx::UserPass
    {
    public:
        const char * getClassName() const final { return "ImGuiPass"; }

        ImGuiPass();
        ~ImGuiPass();

        void setup(const gfx::RenderContext & _renderContext, double _dt) override;
        void draw(const gfx::RenderContext & _renderContext, gfx::CommandList * _cmdList) const override;

    private:
        static const vg::engine::IEngine * getEngine();

        void displayEngineWindow();
        void displayRendererWindow();
        
    private:
        // Plugins
        bool m_isEngineWindowVisible = false;
        bool m_isRendererWindowVisible = false;
    };
}