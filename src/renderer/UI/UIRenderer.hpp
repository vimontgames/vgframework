#include "UIRenderer.h"
#include "gfx/ITexture.h"
#include "renderer/IImGuiAdapter.h"
#include "renderer/Renderer.h"
#include "renderer/Options/RendererOptions.h"
#include "renderer/RenderPass/ImGui/ImGui.h"
#include "renderer/RenderPass/ImGui/Extensions/FontStyle/ImGuiFontStyleExtension.h"
#include "editor/ImGui/Extensions/imGuiExtensions.h" 
#include "Shaders/system/packing.hlsli"
#include "Shaders/system/picking.hlsli"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    UIRenderer::UIRenderer(gfx::IViewport * _viewport, gfx::IView * _view) :
        m_viewport(_viewport),
        m_view(_view)
    {
    
    }

    //--------------------------------------------------------------------------------------
    UIRenderer::~UIRenderer()
    {
  
    }

    //--------------------------------------------------------------------------------------
    void UIRenderer::Add(const UIElement & _desc)
    {
        m_uiElements.push_back(_desc);
    }

    //--------------------------------------------------------------------------------------
    // Create transparent window to render game UI
    //--------------------------------------------------------------------------------------
    void UIRenderer::RenderFullscreen()
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0); // This is necessary in !VG_ENABLE_EDITOR mode to remove dark window borders

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
        ImGui::PopStyleVar(2);
    }

    //--------------------------------------------------------------------------------------
    void UIRenderer::RenderWindowed()
    {
        render();
    }

    //--------------------------------------------------------------------------------------
    core::uint2 UIRenderer::getSize() const
    {
        return m_view ? m_view->GetSize() : m_viewport->GetRenderTargetSize();
    }

    //--------------------------------------------------------------------------------------
    core::float2 UIRenderer::getScale() const
    {
        return m_view ? m_view->GetViewportScale() : float2(1, 1);
    }

    //--------------------------------------------------------------------------------------
    core::float2 UIRenderer::getOffset() const
    {
        return m_view ? m_view->GetViewportOffset() : float2(0, 0);
    }

    //--------------------------------------------------------------------------------------
    void UIRenderer::Clear()
    {
        m_uiElements.clear();
    }

    //--------------------------------------------------------------------------------------
    void UIRenderer::render()
    {
        VG_PROFILE_CPU("RenderUI");

        sort(m_uiElements.begin(), m_uiElements.end(), [](UIElement & a, UIElement & b)
        {
            return a.m_item.m_matrix[3].z > b.m_item.m_matrix[3].z;
        }
        );

        auto * imGuiAdapter = Renderer::get()->GetImGuiAdapter();
        const RendererOptions * options = RendererOptions::get();
        const bool debugUI = options->isDebugUIEnabled();

        const uint2 size = getSize();
        const float2 scale = getScale();
        const float2 offset = getOffset();

        float2 screenSizeInPixels = ImVec2ToFloat2(ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin());
        float2 viewSizeInPixels = screenSizeInPixels * scale;
        float2 windowOffset = ImVec2ToFloat2(ImGui::GetCursorPos()) + screenSizeInPixels * offset;

        ImVec2 clipOffset = ImGui::GetWindowPos() + ImVec2(windowOffset.x, windowOffset.y);
        ImVec2 viewClipSize = ImVec2((float)size.x+1.0f, (float)size.y+ 1.0f);

        ImGui::PushClipRect(clipOffset, clipOffset + viewClipSize, true);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

        float4x4 viewProj = float4x4::identity();
        if (m_view)
            viewProj = mul(m_view->GetViewInvMatrix(), m_view->GetProjectionMatrix());
  
        for (uint i = 0; i < m_uiElements.size(); ++i)
        {
            const UIElement & elem = m_uiElements[i];

            UICanvas canvas;
            float4 worldPos = (float4)0.0f;

            if (elem.m_canvas)
            {
                canvas = *elem.m_canvas;

                if (canvas.m_canvasType == CanvasType::CanvasType_3D)
                {
                    worldPos = mul(float4(canvas.m_matrix[3].xyz + canvas.m_offset.xyz, 1.0f), viewProj);
                    worldPos.xyz = worldPos.xyz / worldPos.w;
                    worldPos.xy = worldPos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
                    worldPos.xy *= viewSizeInPixels.xy; 
                    //VG_INFO("[UI] worldPos = (%.2f, %.2f)", (float)worldPos.x, (float)worldPos.y);
                }
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
            float2 canvasOffset = canvas.m_matrix[3].xy + float2(1, 1);

            // Align canvas
            if (canvas.m_canvasType == CanvasType::CanvasType_3D)
            {
                canvasOffset.xy += worldPos.xy - canvasSizeInPixel.xy * 0.5f;
            }
            else
            {
                switch (canvas.m_alignX)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(canvas.m_alignX);
                        break;

                    case HorizontalAligment::Left:
                        canvasOffset.x += 0;
                        break;

                    case HorizontalAligment::Center:
                        canvasOffset.x += viewSizeInPixels.x * 0.5f - canvasSizeInPixel.x * 0.5f;
                        break;

                    case HorizontalAligment::Right:
                        canvasOffset.x += viewSizeInPixels.x - canvasSizeInPixel.x;
                        break;
                }

                switch (canvas.m_alignY)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(canvas.m_alignX);
                        break;

                    case VerticalAligment::Top:
                        canvasOffset.y += 0;
                        break;

                    case VerticalAligment::Center:
                        canvasOffset.y += viewSizeInPixels.y * 0.5f - canvasSizeInPixel.y * 0.5f;
                        break;

                    case VerticalAligment::Bottom:
                        canvasOffset.y += viewSizeInPixels.y - canvasSizeInPixel.y;
                        break;
                }
            }

            float2 canvasRect[2] =
            {
                canvasOffset,
                canvasOffset + canvasSizeInPixel
            };

            if (debugUI && elem.m_type == UIElementType::Canvas)
            {
                ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(winOffset + canvasRect[0]), float2ToImVec2(winOffset + canvasRect[1]), packRGBA8(canvas.m_color));
                continue;
            }

            float2 elemSize = elem.m_item.m_size;
            float2 elemPos = canvasOffset;

            //if (canvas.m_canvasType != CanvasType::CanvasType_3D)
                elemPos += elem.m_item.m_offset.xy;

            switch (elem.m_type)
            {
                case UIElementType::Text:
                    imGuiAdapter->PushFont(elem.m_font, elem.m_style);
                    break;
            }

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

            float2 center = elem.m_item.m_center * elemSize;

            switch (elem.m_item.m_alignX)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_item.m_alignX);
                    break;

                case HorizontalAligment::Left:
                    elemPos.x += center.x - elemSize.x * 0.5f;
                    break;

                case HorizontalAligment::Center:
                    elemPos.x += canvasSizeInPixel.x * 0.5f - center.x;
                    break;

                case HorizontalAligment::Right:
                    elemPos.x += canvasSizeInPixel.x - center.x;
                    break;
            }

            switch (elem.m_item.m_alignY)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(elem.m_item.m_alignY);
                    break;

                case VerticalAligment::Top:
                    elemPos.y += center.y - elemSize.y * 0.5f;
                    break;

                case VerticalAligment::Center:
                    elemPos.y += canvasSizeInPixel.y * 0.5f - center.y * 0.5f;
                    break;

                case VerticalAligment::Bottom:
                    elemPos.y += canvasSizeInPixel.y - center.y;
                    break;
            }

            float2 elemRect[2] =
            {
                elemPos,
                elemPos + center
            };

            switch (elem.m_type)
            {
                case UIElementType::Image:
                {
                    if (elem.m_texture)
                    {
                        ImGui::SetCursorPos(float2ToImVec2(elemPos.xy + windowOffset));
                        ImTextureID texID = imGuiAdapter->GetTextureID(elem.m_texture);

                        ImGui::Image(texID, float2ToImVec2(elemSize), ImVec2(0, 0), ImVec2(1, 1), float4ToImVec4(elem.m_item.m_color));

                        // Picking on Viewport not supported yet
                        if (m_view && m_view->IsToolmode())
                        {
                            if (ImGui::IsItemHovered())
                            {
                                PickingHit hit;
                                hit.m_id = elem.m_item.m_pickingID;
                                hit.m_pos = float4(elemPos.xy, 0, 1);
                                m_view->AddPickingHit(hit);
                            }
                        }

                        imGuiAdapter->ReleaseTextureID(elem.m_texture);
                    }
                    break;
                }

                case UIElementType::Text:
                {
                    ImGui::SetCursorPos(float2ToImVec2(elemPos.xy + windowOffset));
                    ImGui::TextColored(float4ToImVec4(elem.m_item.m_color), elem.m_text.c_str());
                    
                    // Picking on Viewport not supported yet
                    if (m_view && m_view->IsToolmode())
                    {
                        if (ImGui::IsItemHovered())
                        {
                            PickingHit hit;
                            hit.m_id = elem.m_item.m_pickingID;
                            hit.m_pos = float4(elemPos.xy + center.xy, 0, 1); // TODO: pass mouse position instead?
                            m_view->AddPickingHit(hit);
                        }
                    }

                    imGuiAdapter->PopFont();
                }
            }

            if (debugUI)
            {
                switch (elem.m_type)
                {
                    case UIElementType::Image:
                        ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(winOffset + elemRect[0]), float2ToImVec2(winOffset + elemRect[1]), packRGBA8(elem.m_item.m_color * 0.5f));
                        break;

                    case UIElementType::Text:
                        ImGui::GetForegroundDrawList()->AddRect(float2ToImVec2(winOffset + elemRect[0]), float2ToImVec2(winOffset + elemRect[1]), packRGBA8(elem.m_item.m_color*0.5f));
                        break;
                }
            }
        }

        ImGui::PopStyleColor();
        ImGui::PopClipRect();
        m_uiElements.clear();
    }
}