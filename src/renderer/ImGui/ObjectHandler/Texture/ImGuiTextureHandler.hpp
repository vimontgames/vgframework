#pragma once

#include "gfx/Resource/Texture.h"
#include "renderer/Renderer.h"
#include "renderer/Imgui/imguiAdapter.h"

namespace vg::renderer
{
    using namespace vg::core;

    class ImguiTextureHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            // Texture preview (WIP)
            auto * tex = (gfx::Texture *)_object;
            auto imGuiAdapter = Renderer::get()->getImGuiAdapter();
            ImTextureID texID = imGuiAdapter->getImguiTextureID(tex);
            {
                ImGui::Image(texID, ImVec2(128, 128));
            }
            imGuiAdapter->releaseImguiTextureID(texID);
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Texture, ImguiTextureHandler);
}



