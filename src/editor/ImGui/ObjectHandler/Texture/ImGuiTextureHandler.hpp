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
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            bool changed = false;
            auto * originalTex = dynamic_cast<gfx::ITexture*>(_object);
            if (originalTex)
            {
                auto imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
                {
                    auto availableWidth = ImGui::GetContentRegionAvail().x;
                    auto texturePreviewSize = ImGuiWindow::getImGuiPreviewSize();

                    ImGui::PushItemWidth(availableWidth);
                    {
                        gfx::ITexture * texture = nullptr;

                        if (originalTex->GetTextureType() == gfx::TextureType::Texture2D)
                        {
                            // Texture can be displayed directly
                            texture = originalTex;
                        }
                        else
                        {
                            // TODO
                            texture = imGuiAdapter->GetPreviewTexture(originalTex);
                        }

                        if (texture != nullptr)
                        {
                            ImTextureID texID = imGuiAdapter->GetTextureID(texture);
                            ImGui::Image(texID, texturePreviewSize);

                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                            {
                                if (ImGui::BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None))
                                {
                                    string tooltip;
                                    
                                    switch (originalTex->GetTextureType())
                                    {
                                        default:
                                        case gfx::TextureType::Texture2D:
                                            tooltip = fmt::sprintf("%ux%u\n", originalTex->GetWidth(), originalTex->GetHeight());
                                            break;

                                        case gfx::TextureType::TextureCube:
                                            tooltip = fmt::sprintf("%ux%ux%u\n", originalTex->GetWidth(), originalTex->GetHeight(), originalTex->GetSlices());
                                            break;
                                    }

                                    tooltip += fmt::sprintf("%s\n%u mip levels(s)", asString(originalTex->GetPixelFormat()), originalTex->GetMipmapCount());
                                    
                                    ImGui::Text(tooltip.c_str());
                                    ImGui::EndTooltip();
                                }
                            }

                            imGuiAdapter->ReleaseTextureID(texture);
                        }
                        else
                        {
                            ImDrawList * draw_list = ImGui::GetWindowDrawList();
                            ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
                            draw_list->AddRectFilled(cursor_pos, cursor_pos + texturePreviewSize, IM_COL32(0, 0, 0, 255));
                            ImGui::Dummy(texturePreviewSize);
                        }
                    }
                    ImGui::PopItemWidth();
                }
            }

            return changed;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Texture, ImGuiTextureHandler);
}



