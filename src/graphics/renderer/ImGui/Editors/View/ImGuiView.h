#pragma once

#include "graphics/renderer/ImGui/Editors/ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImGuiView : public ImguiEditor
    {
    public:
                                        ImGuiView   (const string & _name, Flags _flags);
                                        ~ImGuiView  ();

        void                            update      (double _dt) final override;
        void                            display     () final override;

    private:
        core::uint2                     m_size = core::uint2(0, 0);
        graphics::driver::IView *       m_view = nullptr;
        graphics::driver::ITexture *    m_texture = nullptr;
        ImTextureID texID;
    };
}