#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::engine
{
    class IEngine;
}

namespace vg::graphics::renderer
{
    class ImguiEditor;

    //--------------------------------------------------------------------------------------
    class ImguiPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "ImguiPass"; }

        ImguiPass();
        ~ImguiPass();

        void setup(const driver::FrameGraph::RenderContext & _renderContext, double _dt) override;
        void draw(const driver::FrameGraph::RenderContext & _renderContext, driver::CommandList * _cmdList) const override;

    private:
        static const vg::engine::IEngine * getEngine();

        void displayEngineWindow();
        void displayRendererWindow();

        template <class T> T * getEditorWindow();
        
    private:
        // Plugins
        bool m_isEngineWindowVisible = false;
        bool m_isRendererWindowVisible = false;

        // Editor windows
        core::vector<ImguiEditor *> m_editorWindows;
    };
}