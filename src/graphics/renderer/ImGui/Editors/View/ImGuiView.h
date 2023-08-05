#pragma once

#include "graphics/renderer/ImGui/Editors/ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImGuiView : public ImguiEditor
    {
    public:
                                        ImGuiView   (const string & _name, Flags _flags, gfx::ViewType _viewType);
                                        ~ImGuiView  ();

        void                            update      (double _dt) final override;
        void                            display     () final override;

    private:
        gfx::ViewType                m_viewType = gfx::ViewType::Game;
        core::uint2                     m_size = core::uint2(0, 0);
        gfx::IView *       m_view = nullptr;
        gfx::ITexture *    m_texture = nullptr;
    };
}