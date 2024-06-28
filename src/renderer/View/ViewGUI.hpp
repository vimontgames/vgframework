#include "ViewGUI.h"
#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ViewGUI::ViewGUI(gfx::IView * _view) :
        m_view(_view)
    {
    
    }

    //--------------------------------------------------------------------------------------
    ViewGUI::~ViewGUI()
    {
  
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::AddText(renderer::PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, HorizontalAligment _alignX, VerticalAligment _alignY, const core::string & _text)
    {
        m_uiElements.push_back(UIElement(_pickingID, _matrix, _color, _alignX, _alignY, _text));
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::AddImage(renderer::PickingID _pickingID, const core::float4x4 & _matrix, core::float4 _color, HorizontalAligment _alignX, VerticalAligment _alignY, const gfx::ITexture * _texture)
    {
        m_uiElements.push_back(UIElement(_pickingID, _matrix, _color, _alignX, _alignY, _texture));
    }

    //--------------------------------------------------------------------------------------
    // Create transparent window to render game UI
    //--------------------------------------------------------------------------------------
    void ViewGUI::RenderFullscreen()
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("ViewGUI", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize); // create a window
        {
            render();
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::RenderWindowed()
    {
        render();
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::render()
    {
        sort(m_uiElements.begin(), m_uiElements.end(), [](UIElement & a, UIElement & b)
        {
            return a.m_matrix[3].z > b.m_matrix[3].z;
        }
        );

        auto * imGuiAdapter = Renderer::get()->GetImGuiAdapter();

        float2 windowOffset = ImVec2ToFloat2(ImGui::GetCursorPos());

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
        //ImGui::BeginChild("ViewGUIFrame");
        {
            float2 pos = ImVec2ToFloat2(GImGui->CurrentWindow->Pos) + windowOffset;
            float2 size = ImVec2ToFloat2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
            //ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(pos), float2ToImVec2(pos+size), 0xFF0000FF);

            for (uint i = 0; i < m_uiElements.size(); ++i)
            {
                const auto elem = m_uiElements[i];

                float3 pos = (float3)0.0f;

                switch (elem.m_alignX)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_alignX);
                        break;

                    case HorizontalAligment::Left:
                        pos.x = 0;
                        break;

                    case HorizontalAligment::Center:
                        pos.x = size.x * 0.5f;
                        break;

                    case HorizontalAligment::Right:
                        pos.x = size.x;
                        break;
                }

                switch (elem.m_alignY)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_alignY);
                        break;

                    case VerticalAligment::Top:
                        pos.y = 0.0f;
                        break;

                    case VerticalAligment::Center:
                        pos.y = size.y * 0.5f;
                        break;

                    case VerticalAligment::Bottom:
                        pos.y = size.y;
                        break;
                }

                pos += elem.m_matrix[3].xyz;

                switch (elem.m_type)
                {
                    default:VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_type);
                        break;

                    case UIElementType::Text:
                    {
                        const auto textSize = ImVec2ToFloat2(ImGui::CalcTextSize(elem.m_text.c_str()));
                        pos.xy -= textSize / 2.0f;
                        //pos.y = size.y - pos.y - 1;

                        ImGui::SetCursorPos(float2ToImVec2(pos.xy + windowOffset));
                        ImGui::TextColored(float4ToImVec4(elem.m_color.rgba), elem.m_text.c_str());

                        if (ImGui::IsItemHovered())
                        {
                            PickingHit hit;
                            hit.m_id = elem.m_pickingID;
                            hit.m_pos = float4(pos, 1);
                            m_view->AddPickingHit(hit);
                        }
                    }
                    break;

                    case UIElementType::Image:
                    {
                        if (elem.m_texture)
                        {
                            float2 texSize = float2(elem.m_texture->GetWidth(), elem.m_texture->GetHeight());
                            texSize.x *= elem.m_matrix[0].x;
                            texSize.y *= elem.m_matrix[1].y;
                            pos.xy -= texSize / 2.0f;
                            //pos.y = size.y - pos.y - 1;

                            ImGui::SetCursorPos(float2ToImVec2(pos.xy + windowOffset));
                            ImTextureID texID = imGuiAdapter->GetTextureID(elem.m_texture);

                            ImGui::Image(texID, float2ToImVec2(texSize), ImVec2(0, 0), ImVec2(1, 1), float4ToImVec4(elem.m_color));

                            if (ImGui::IsItemHovered())
                            {
                                PickingHit hit;
                                hit.m_id = elem.m_pickingID;
                                hit.m_pos = float4(pos, 1);
                                m_view->AddPickingHit(hit);
                            }

                            imGuiAdapter->ReleaseTextureID(elem.m_texture);
                        }
                    }
                    break;
                }
            }
        }
        //ImGui::EndChild();
        ImGui::PopStyleColor();
        m_uiElements.clear();
    }
}