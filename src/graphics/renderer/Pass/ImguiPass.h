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
        ObjectPointer
    };

    //--------------------------------------------------------------------------------------
    class ImguiPass : public driver::UserPass
    {
    public:
        const char * getClassName() const final { return "ImguiPass"; }

        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        const vg::engine::IEngine * getEngine() const;

        void displayOptionsWindow();
        void displayEngineWindow();
        void displayRendererWindow();
        void displaySceneWindow();
        void displayCurrentSelectionWindow();
        void displayPerformanceWindow(double _dt);

        void displayArrayObject(IObject * _object, core::uint _index, core::u32 _color, const char * _name, UIMode _mode = UIMode::All);
        void displayObject(core::IObject * _object, UIMode _mode = UIMode::All);

        void updateSelection(core::IObject * _object, UIMode _mode);
        void setCurrentSelection(core::IObject * _object);

    private:
        bool m_isDisplayOptionsWindowsVisible = true;

        bool m_isEngineWindowVisible = true;
        bool m_isRendererWindowVisible = true;
        bool m_isPerfWindowVisible = true;
        bool m_isSceneWindowVisible = true;
        bool m_isCurrentSelectionWindowVisible = true;
        bool m_isAboutWindowVisible = false;

        core::IObject * m_selected = nullptr;

        core::uint  m_captureFrameCounter = 0;

        double      m_accum = 0.0f;
        core::uint  m_frame = 0;
        float       m_dt = 0.0f;
        float       m_fps = 0.0f;
    };
}