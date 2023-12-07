#pragma once

namespace vg::editor
{
    class ImGuiMenu
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

        virtual ~ImGuiMenu() = default;
        virtual Status  Display(core::IObject * _object) = 0;

    protected:
        core::uint      m_selected  = 0;
        const char *    m_popup = nullptr;
        const char *    m_popupIcon = nullptr;
    };
}