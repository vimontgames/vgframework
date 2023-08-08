#pragma once

#include "gfx/ITexture.h"
#include "renderer/IRenderer.h"
#include "renderer/IImGuiAdapter.h"
#include "editor/Editor.h"

namespace vg::editor
{
    using namespace vg::core;

    class ImGuiTextureHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            // Texture preview
            auto * tex = dynamic_cast<gfx::ITexture*>(_object);
            if (tex)
            {
                auto imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
                ImTextureID texID = imGuiAdapter->GetTextureID(tex);
                {
                    ImGui::Image(texID, ImVec2(128, 128));
                }
                imGuiAdapter->ReleaseTextureID(texID);
            }
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Texture, ImGuiTextureHandler);
}



