#include "ViewGUI.h"
#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/IImGuiAdapter.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ViewGUI::ViewGUI()
    {
    
    }

    //--------------------------------------------------------------------------------------
    ViewGUI::~ViewGUI()
    {
  
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::AddText(const core::float4 & _rect, float _depth, const core::string & _text, core::float4 _color)
    {
        m_uiElements.push_back(UIElement(_rect, _depth, _text, _color));
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::AddImage(const core::float4 & _rect, float _depth, const gfx::ITexture * _texture, core::float4 _color)
    {
        m_uiElements.push_back(UIElement(_rect, _depth, _texture, _color));
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
            return a.m_depth > b.m_depth;
        }
        );

        auto * imGuiAdapter = Renderer::get()->GetImGuiAdapter();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
        ImGui::BeginChild("ViewGUIFrame");
        {
            float2 pos = ImVec2ToFloat2(GImGui->CurrentWindow->Pos);
            float2 size = ImVec2ToFloat2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
            ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(pos), float2ToImVec2(pos+size), 0xFF00FF00);

            for (uint i = 0; i < m_uiElements.size(); ++i)
            {
                const auto elem = m_uiElements[i];

                switch (elem.m_type)
                {
                    default:VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_type);
                        break;

                    case UIElementType::Text:
                        if (!all(elem.m_rect == 0))
                            ImGui::SetCursorPos(float2ToImVec2(elem.m_rect.xy * size));
                        ImGui::TextColored(float4ToImVec4(elem.m_color.rgba), elem.m_text.c_str());
                        break;

                    case UIElementType::Image:
                        if (elem.m_texture)
                        {
                            if (!all(elem.m_rect == 0))
                                ImGui::SetCursorPos(float2ToImVec2(elem.m_rect.xy * size));
                            ImTextureID texID = imGuiAdapter->GetTextureID(elem.m_texture);
                            ImGui::Image(texID, float2ToImVec2((elem.m_rect.zw-elem.m_rect.xy) * size), ImVec2(0, 0), ImVec2(1, 1), float4ToImVec4(elem.m_color));
                        }
                        break;
                }
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        m_uiElements.clear();
    }
}