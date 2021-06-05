#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::engine
{
    class IEngine;
}

namespace vg::graphics::renderer
{
    enum class UIMode
    {
        All = 0,
        Scene,
        Resources,
        Object
    };

    enum class UIWindow
    {
        FPS = 0,
        Platform,
        Resources,
        Scene,
        Selection,
        Shaders
    };

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

        void displaySceneWindow();
        void displayCurrentSelectionWindow();
        void displayFpsWindow();
        void displayPlatformWindow();
        void displayShadersWindow();
        void displayResourcesWindow();

        void displayOptionsWindow();

        void displayAboutWindow();

        void displayArrayObject(IObject * _object, core::uint _index, core::u32 _color, const char * _name, UIMode _mode = UIMode::All);
        void displayObject(core::IObject * _object, UIMode _mode = UIMode::All);

        void updateSelection(core::IObject * _object, UIMode _mode);
        void setCurrentSelection(core::IObject * _object);

    private:
        // Plugins
        bool m_isEngineWindowVisible = false;
        bool m_isRendererWindowVisible = false;

        // Windows
        bool m_isWindowVisible[core::enumCount<UIWindow>()];

        // Options
        bool m_isDisplayOptionsWindowsVisible = true;
        
        // About
        bool m_isAboutWindowVisible = false;

        core::IObject * m_selected = nullptr;
        core::uint  m_captureFrameCounter = 0;

        double      m_accum = 0.0f;
        core::uint  m_frame = 0;
        float       m_dt = 0.0f;
        float       m_fps = 0.0f;
    };
}