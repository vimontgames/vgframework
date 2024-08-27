#include "ViewGUI.h"
#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/IImGuiAdapter.h"
#include "Shaders/system/packing.hlsli"

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
    void ViewGUI::AddCanvas(const gfx::UICanvas * _canvas, const UIItem & _desc)
    {
        m_uiElements.push_back(UIElement(_canvas, _desc));
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::AddText(const gfx::UICanvas * _canvas, const UIItem & _desc, const core::string & _text)
    {
        m_uiElements.push_back(UIElement(_canvas, _desc, _text));
    }

    //--------------------------------------------------------------------------------------
    void ViewGUI::AddImage(const gfx::UICanvas * _canvas, const UIItem & _desc, const gfx::ITexture * _texture)
    {
        m_uiElements.push_back(UIElement(_canvas, _desc, _texture));
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

        #if !VG_ENABLE_EDITOR
        // if not rendered from inside dock the ImGui::Begin part is needed!
        ImGui::Begin("ViewGUI", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        #endif
        {
            ImGui::SetCursorScreenPos(ImVec2(0, 0));
            render();
        }
        #if !VG_ENABLE_EDITOR
        ImGui::End();
        #endif
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
            return a.m_item.m_matrix[3].z > b.m_item.m_matrix[3].z;
        }
        );

        auto * imGuiAdapter = Renderer::get()->GetImGuiAdapter();
        const RendererOptions * options = RendererOptions::get();
        const bool debugUI = options->isDebugUIEnabled();

        float2 screenSizeInPixels = ImVec2ToFloat2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
        float2 viewSizeInPixels = screenSizeInPixels * m_view->GetViewportScale();
        float2 windowOffset = ImVec2ToFloat2(ImGui::GetCursorPos()) + screenSizeInPixels * m_view->GetViewportOffset();

        ImVec2 clipOffset = ImGui::GetWindowPos() + ImVec2(windowOffset.x, windowOffset.y);
        ImVec2 viewSize = ImVec2((float)m_view->GetSize().x+1.0f, (float)m_view->GetSize().y+ 1.0f);

        ImGui::PushClipRect(clipOffset, clipOffset + viewSize, true);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
  
        for (uint i = 0; i < m_uiElements.size(); ++i)
        {
            const auto elem = m_uiElements[i];            

            UICanvas canvas;
            if (elem.m_canvas)
            {
                canvas = *elem.m_canvas;
            }
            else
            {
                // Default canvas
                canvas.m_size = viewSizeInPixels;
                canvas.m_alignX = HorizontalAligment::Center;
                canvas.m_alignY = VerticalAligment::Center;
            }

            // Compute Canvas Rect
            float2 canvasSizeInPixel = (float2)canvas.m_size;
            if (asBool(canvas.m_flags & UIItemFlags::AutoResize))
            {
                canvasSizeInPixel = (float2)canvas.m_size * viewSizeInPixels / (float2)canvas.m_resolution;
            }                

            float2 winOffset = ImVec2ToFloat2(GImGui->CurrentWindow->Pos) + windowOffset;
            float2 offset = canvas.m_matrix[3].xy + float2(1, 1);

            // Align canvas
            switch (canvas.m_alignX)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(canvas.m_alignX);
                    break;
                
                case HorizontalAligment::Left:
                    offset.x += 0;
                    break;
                
                case HorizontalAligment::Center:
                    offset.x += viewSizeInPixels.x * 0.5f - canvasSizeInPixel.x * 0.5f;
                    break;
                
                case HorizontalAligment::Right:
                    offset.x += viewSizeInPixels.x * (1.0f - (float)canvas.m_size.x * 0.01f);
                    break;
            }
                
            switch (canvas.m_alignY)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(canvas.m_alignX);
                    break;

                case VerticalAligment::Top:
                    offset.y += 0;
                    break;

                case VerticalAligment::Center:
                    offset.y += viewSizeInPixels.y * 0.5f - canvasSizeInPixel.y * 0.5f;
                    break;

                case VerticalAligment::Bottom:
                    offset.y += viewSizeInPixels.y * (1.0f - (float)canvas.m_size.y * 0.01f);
                    break;
            }

            float2 canvasRect[2] =
            {
                offset,
                offset + canvasSizeInPixel
            };

            if (debugUI)
                ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(winOffset+canvasRect[0]), float2ToImVec2(winOffset+canvasRect[1]), packRGBA8(canvas.m_color));

            float2 elemSize = elem.m_item.m_size;
            float2 elemPos = offset + elem.m_item.m_matrix[3].xy;

            if (asBool(UIItemFlags::AutoResize & elem.m_item.m_flags))
            {
                switch (elem.m_type)
                {
                    case UIElementType::Image:
                        if (elem.m_texture)
                            elemSize = float2(elem.m_texture->GetWidth(), elem.m_texture->GetHeight()) * float2(elem.m_item.m_matrix[0].x, elem.m_item.m_matrix[1].y);
                        break;

                    case UIElementType::Text:
                        elemSize = ImVec2ToFloat2(ImGui::CalcTextSize(elem.m_text.c_str()));
                        break;
                }
            }

            switch (elem.m_item.m_alignX)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_item.m_alignX);
                    break;

                case HorizontalAligment::Left:
                    elemPos.x += 0;
                    break;

                case HorizontalAligment::Center:
                    elemPos.x += canvasSizeInPixel.x * 0.5f - elemSize.x * 0.5f;
                    break;

                case HorizontalAligment::Right:
                    elemPos.x += canvasSizeInPixel.x - elemSize.x;
                    break;
            }

            switch (elem.m_item.m_alignY)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_item.m_alignY);
                    break;

                case VerticalAligment::Top:
                    elemPos.y += 0;
                    break;

                case VerticalAligment::Center:
                    elemPos.y += canvasSizeInPixel.y * 0.5f - elemSize.y * 0.5f;
                    break;

                case VerticalAligment::Bottom:
                    elemPos.y += canvasSizeInPixel.y - elemSize.y;
                    break;
            }

            float2 elemRect[2] =
            {
                elemPos,
                elemPos + elemSize
            };

            switch (elem.m_type)
            {
                case UIElementType::Image:
                {
                    if (elem.m_texture)
                    {
                        ImGui::SetCursorPos(float2ToImVec2(elemPos.xy - elemSize * 0.5f + windowOffset));
                        ImTextureID texID = imGuiAdapter->GetTextureID(elem.m_texture);

                        ImGui::Image(texID, float2ToImVec2(elemSize), ImVec2(0, 0), ImVec2(1, 1), float4ToImVec4(elem.m_item.m_color));

                        if (ImGui::IsItemHovered())
                        {
                            PickingHit hit;
                            hit.m_id = elem.m_item.m_pickingID;
                            hit.m_pos = float4(elemPos.xy, 0, 1);
                            m_view->AddPickingHit(hit);
                        }

                        imGuiAdapter->ReleaseTextureID(elem.m_texture);
                    }
                    break;
                }

                case UIElementType::Text:
                {
                    ImGui::SetCursorPos(float2ToImVec2(elemPos.xy + windowOffset - float2(0,elemSize.y * 0.5f)));
                    ImGui::TextColored(float4ToImVec4(elem.m_item.m_color), elem.m_text.c_str());
                        
                    if (ImGui::IsItemHovered())
                    {
                        PickingHit hit;
                        hit.m_id = elem.m_item.m_pickingID;
                        hit.m_pos = float4(elemPos.xy, 0, 1);
                        m_view->AddPickingHit(hit);
                    }
                }
            }

            if (debugUI)
            {
                switch (elem.m_type)
                {
                case UIElementType::Image:
                    ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(winOffset + elemRect[0] - elemSize * 0.5f), float2ToImVec2(winOffset + elemRect[1] - elemSize * 0.5f), packRGBA8(elem.m_item.m_color * 0.5f));
                    break;

                case UIElementType::Text:
                    ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(winOffset + elemRect[0] - float2(0,elemSize.y * 0.5f)), float2ToImVec2(winOffset + elemRect[1] - float2(0, elemSize.y * 0.5f)), packRGBA8(elem.m_item.m_color*0.5f));
                    break;
                }
            }
        }

        ImGui::PopStyleColor();
        ImGui::PopClipRect();
        m_uiElements.clear();
    }
}