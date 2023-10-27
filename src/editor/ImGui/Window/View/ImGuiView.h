#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiView : public ImGuiWindow
    {
    public:
                            ImGuiView   (const char * _icon, const core::string & _path, const core::string & _name, Flags _flags, gfx::ViewTarget _target);
                            ~ImGuiView  ();

        void                DrawGUI     () final override;

    private:
        gfx::ViewTarget     m_target    = gfx::ViewTarget::Game;
        core::uint2         m_size      = core::uint2(0, 0);
        gfx::IView *        m_view      = nullptr;
        gfx::ITexture *     m_texture   = nullptr;
    };
}