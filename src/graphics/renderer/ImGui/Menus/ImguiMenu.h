#pragma once

#include "graphics/renderer/ImGui/Imgui_Consts.h"

namespace vg::graphics::renderer
{
    class ImguiMenu
    {
    public:
        enum class Status
        {
            None = 0,
            Canceled,
            Success,
            Failure,
            Opened,
            Saved,
            Removed
        };

        virtual Status  Display(core::IObject * _object) = 0;

    protected:
        core::uint      m_selected  = 0;
        const char *    m_popup;
    };
}