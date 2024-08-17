#pragma once

namespace vg::editor
{
    struct ImGuiConsoleOptions
    {
        core::LevelFlags        m_levels = (core::LevelFlags::Warning | core::LevelFlags::Error);
    };
}