#pragma once

#include "graphics/driver/Resource/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Imgui/imguiAdapter.h"

namespace vg::graphics::renderer
{
    using namespace vg::core;

    class ImguiTextureHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            // Texture preview (WIP)
            auto * tex = (driver::Texture *)_object;
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



