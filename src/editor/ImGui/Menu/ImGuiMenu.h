#pragma once

#include "core/Object/Object.h"

namespace vg::editor
{
    class ImGuiMenu : public core::Object
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
        virtual Status  Display(core::IObject * _object) { return Status::None; }

    protected:
        core::uint      m_selected  = 0;
        core::string    m_popup;
        core::IObject * m_popupObject = nullptr;
        const char *    m_popupIcon = nullptr;
    };
}