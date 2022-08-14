#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::core
{
    class ISector;
}

namespace vg::engine
{
    class IEngine;
}

namespace vg::graphics::renderer
{
    enum class UIWindow
    {
        FPS = 0,
        Platform,
        Shaders
    };

    class ImguiEditor;

    //--------------------------------------------------------------------------------------
    class ImguiPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "ImguiPass"; }

        ImguiPass();
        ~ImguiPass();

        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        static const vg::engine::IEngine * getEngine();

        void displayEngineWindow();
        void displayRendererWindow();

        void displayFpsWindow();
        void displayPlatformWindow();
        void displayShadersWindow();

        template <class T> T * getEditorWindow();
        
    private:
        // Plugins
        bool m_isEngineWindowVisible = false;
        bool m_isRendererWindowVisible = false;

        // Windows
        bool m_isWindowVisible[core::enumCount<UIWindow>()];
        core::vector<ImguiEditor *> m_editorWindows;
        
        core::uint  m_captureFrameCounter = 0;

        double      m_accum = 0.0f;
        core::uint  m_frame = 0;
        float       m_dt = 0.0f;
        float       m_fps = 0.0f;
    };
}