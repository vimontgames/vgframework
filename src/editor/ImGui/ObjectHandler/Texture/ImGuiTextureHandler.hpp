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
        // Texture preview
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext) final
        {
            bool changed = false;
            auto * tex = dynamic_cast<gfx::ITexture*>(_object);
            if (tex)
            {
                auto imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
                ImTextureID texID = imGuiAdapter->GetTextureID(tex);
                {
                    auto availableWidth = ImGui::GetContentRegionAvail().x;

                    // Anti-flickering hack
                    if (ImGui::GetScrollMaxY() == 0.0f)
                        availableWidth -= ImGui::GetStyle().ScrollbarSize;

                    auto texturePreviewSize = ImVec2(availableWidth - style::label::PixelWidth, availableWidth - style::label::PixelWidth);
                    ImGui::PushItemWidth(availableWidth);
                    {
                        ImGui::Image(texID, texturePreviewSize);
                    }
                    ImGui::PopItemWidth();
                }
                imGuiAdapter->ReleaseTextureID(tex);
            }

            return changed;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Texture, ImGuiTextureHandler);
}



