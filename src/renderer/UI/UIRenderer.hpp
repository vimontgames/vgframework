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
    UIRenderer::UIRenderer(IViewport * _viewport, IView * _view) :
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
        m_uiElements.push_back({ _desc, (uint)m_uiElements.size() });
    }

    //--------------------------------------------------------------------------------------
    // Create transparent window to render game UI
    //--------------------------------------------------------------------------------------
    void UIRenderer::RenderFullscreen(RenderUIType _renderUIType)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0); // This is necessary in !VG_ENABLE_EDITOR mode to remove dark window borders

        const bool editor = Renderer::get()->IsEditor();
        if (!editor)
        {
            // if not rendered from inside dock the ImGui::Begin part is needed!
            ImGui::Begin("ViewGUI", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
        }

        ImGui::SetCursorScreenPos(ImVec2(0, 0));
        render(_renderUIType);
        
        // Fix ImGui error "Code uses SetCursorPos()/SetCursorScreenPos() to extend window/parent boundaries." when editor=false 
        ImGui::Dummy(ImVec2(0,0));

        if (!editor)
            ImGui::End();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    //--------------------------------------------------------------------------------------
    void UIRenderer::RenderWindowed(RenderUIType _renderUIType)
    {
        render(_renderUIType);
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
    void UIRenderer::render(RenderUIType _renderUIType)
    {
        const RendererOptions * options = RendererOptions::get();
        switch (_renderUIType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_renderUIType);
                break;

            case RenderUIType::RenderUIType_2D:
                if (!options->IsUI2DEnabled())
                    return;
                break;

            case RenderUIType::RenderUIType_3D:
                if (!options->IsUI3DEnabled())
                    return;
                break;
        }

        VG_PROFILE_CPU(_renderUIType == RenderUIType::RenderUIType_2D ? "RenderUI2D" : "RenderUI3D");

        auto * imGuiAdapter = Renderer::get()->GetImGuiAdapter();

        #ifdef VG_FINAL
        const bool debugUI = false;
        #else
        const bool debugUI = options->isDebugUIEnabled();
        #endif

        const uint2 size = getSize();
        const float2 scale = getScale();
        const float2 offset = getOffset();

        float2 screenSizeInPixels = ImVec2ToFloat2(ImGui::GetContentRegionAvail()); 
        float2 viewSizeInPixels = screenSizeInPixels * scale;
        float2 windowOffset = ImVec2ToFloat2(ImGui::GetCursorPos()) + screenSizeInPixels * offset;
        float2 windowPos = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
        float2 clipOffset = windowPos.xy + windowOffset.xy;
        float2 viewClipSize = float2((float)size.x + 1.0f, (float)size.y + 1.0f);
        float2 viewRectMin = clipOffset;
        float2 viewRectMax = clipOffset + viewClipSize;
        ImGui::PushClipRect(float2ToImVec2(viewRectMin), float2ToImVec2(viewRectMax), true);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

        float4x4 viewProj = float4x4::identity();
        float4x4 viewInv = float4x4::identity();
        float2 mousePos = float2(0, 0);

        if (m_view)
        {
            viewInv = m_view->GetViewInvMatrix();
            viewProj = mul(m_view->GetViewMatrix(), m_view->GetProjectionMatrix());
            mousePos = (float2)m_view->GetRelativeMousePos();
        }

        // TODO: sort at insertion?
        sort(m_uiElements.begin(), m_uiElements.end(), [viewInv](UIElementInfo & a, UIElementInfo & b)
        {
            const bool a3D = (a.element.m_canvas && a.element.m_canvas->m_canvasType == CanvasType::CanvasType_3D);
            const bool b3D = (b.element.m_canvas && b.element.m_canvas->m_canvasType == CanvasType::CanvasType_3D);
        
            if (a3D && b3D)
            {
                if (a.element.m_canvas == b.element.m_canvas)
                    return a.index < b.index;
                else
                {
                    // Transform to view space for Z-sort
                    const float4 viewPosA = mul(viewInv, a.element.m_canvas->m_matrix[3]);
                    const float4 viewPosB = mul(viewInv, b.element.m_canvas->m_matrix[3]);

                    return (bool)(viewPosA.z < viewPosB.z);
                }
            }
            else if (!a3D && b3D)
            {
                return false;
            }
            else if (a3D && !b3D)
            {
                return true;
            }
            else
            {
                return a.index < b.index;
            }
        }
        );

        ImDrawList * drawList = ImGui::GetWindowDrawList();
  
        for (uint i = 0; i < m_uiElements.size(); ++i)
        {
            const UIElement & elem = m_uiElements[i].element;

            UICanvas canvas;
            float4 worldPos = (float4)0.0f;

            float2 resolutionScaling = float2(1, 1);

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

            if (elem.m_canvas && asBool(UIItemFlags::ResolutionScalingX & elem.m_item.m_flags))
                resolutionScaling.x = screenSizeInPixels.x / (float)elem.m_canvas->m_resolution.x;

            if (elem.m_canvas && asBool(UIItemFlags::ResolutionScalingY & elem.m_item.m_flags))
                resolutionScaling.y = screenSizeInPixels.y / (float)elem.m_canvas->m_resolution.y;

            // Compute Canvas Rect
            float2 canvasSizeInPixel = (float2)canvas.m_size;
            if (asBool(canvas.m_flags & UIItemFlags::ResolutionScalingX))
                canvasSizeInPixel.x = (float)canvas.m_size.x * viewSizeInPixels.x / (float)canvas.m_resolution.x;

            if (asBool(canvas.m_flags & UIItemFlags::ResolutionScalingY))
                canvasSizeInPixel.y = (float)canvas.m_size.y * viewSizeInPixels.y / (float)canvas.m_resolution.y;

            float2 winOffset = ImVec2ToFloat2(GImGui->CurrentWindow->Pos) + windowOffset;
            float2 canvasOffset = (float2)0.0f;

            // Align canvas
            if (canvas.m_canvasType == CanvasType::CanvasType_3D)
            {
                canvasOffset.xy += worldPos.xy - canvasSizeInPixel.xy * 0.5f;
            }
            else
            {
                canvasOffset.xy += canvas.m_matrix[3].xy;

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
                drawList->AddRect(float2ToImVec2(winOffset + canvasRect[0]), float2ToImVec2(winOffset + canvasRect[1]), packRGBA8(canvas.m_color));
                continue;
            }

            float2 elemSize = elem.m_item.m_size * resolutionScaling.xy;
            float2 elemPos = canvasOffset + elem.m_item.m_offset.xy * resolutionScaling.xy;

            float fontSize = elem.m_fontSize;
            fontSize *= resolutionScaling.y;

            switch (elem.m_type)
            {
                case UIElementType::Text:
                    imGuiAdapter->PushFont(elem.m_font, elem.m_fontStyle, (renderer::FontSize)round(fontSize));
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

            if (asBool(UIItemFlags::KeepAspectRatio & elem.m_item.m_flags))
            {
                switch (elem.m_type)
                {
                    case UIElementType::Image:
                        if (elem.m_texture)
                        {
                            const float originalRatio = (float)elem.m_texture->GetWidth() / (float)elem.m_texture->GetHeight();
                            const float currentRatio = elemSize.x / elemSize.y;
                            
                            if (currentRatio < originalRatio)
                            {
                                elemSize.x *= originalRatio / currentRatio;
                            }
                            else if (currentRatio > originalRatio)
                            {
                                elemSize.y *= currentRatio / originalRatio;
                            }
                        }
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
                    elemPos.x += canvasSizeInPixel.x - center.x * 2.0f;
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
                    elemPos.y += canvasSizeInPixel.y * 0.5f - center.y;
                    break;

                case VerticalAligment::Bottom:
                    elemPos.y += canvasSizeInPixel.y - center.y * 2.0f;
                    break;
            }

            float2 elemRect[2] =
            {
                elemPos,
                elemPos + elemSize
            };

            // Cull offscreen UI elements
            if (any(elemRect[1] > 0) && any(elemRect[0] < viewSizeInPixels))
            {
                switch (elem.m_type)
                {
                    case UIElementType::Image:
                    {
                        if (elem.m_texture)
                        {
                            ImTextureID texID = imGuiAdapter->GetTextureID(elem.m_texture);

                            drawList->AddImage(texID, float2ToImVec2(windowOffset + windowPos + elemRect[0]), float2ToImVec2(windowOffset + windowPos + elemRect[1]), ImVec2(0, 0), ImVec2(1, 1), packRGBA8(elem.m_item.m_color));

                            if (elem.m_canvas && elem.m_canvas->m_canvasType == CanvasType::CanvasType_3D)
                                drawList->AddDrawCmd();

                            // Picking on Viewport not supported yet
                            if (m_view && m_view->IsToolmode())
                            {
                                if (all(mousePos >= elemRect[0]) && all(mousePos <= elemRect[1]))
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
                        drawList->AddText(float2ToImVec2(windowOffset + windowPos + elemRect[0]), packRGBA8(elem.m_item.m_color), elem.m_text.c_str());

                        if (elem.m_canvas && elem.m_canvas->m_canvasType == CanvasType::CanvasType_3D)
                            drawList->AddDrawCmd();

                        // Picking on Viewport not supported yet
                        if (m_view && m_view->IsToolmode())
                        {
                            if (all(mousePos >= elemRect[0]) && all(mousePos <= elemRect[1]))
                            {
                                PickingHit hit;
                                hit.m_id = elem.m_item.m_pickingID;
                                hit.m_pos = float4(mousePos.xy, 0, 1);
                                m_view->AddPickingHit(hit);
                            }
                        }

                        imGuiAdapter->PopFont();
                    }
                }
            }
            else
            {
                switch (elem.m_type)
                {
                    case UIElementType::Text:
                        imGuiAdapter->PopFont();
                        break;
                }
            }

            if (debugUI)
            {
                switch (elem.m_type)
                {
                    case UIElementType::Image:
                        drawList->AddRect(float2ToImVec2(winOffset + elemRect[0]), float2ToImVec2(winOffset + elemRect[1]), packRGBA8(elem.m_item.m_color * 0.5f));
                        break;

                    case UIElementType::Text:
                        drawList->AddRect(float2ToImVec2(winOffset + elemRect[0]), float2ToImVec2(winOffset + elemRect[1]), packRGBA8(elem.m_item.m_color * 0.5f));
                        break;
                }
            }
        }       

        ImGui::SetCursorPos(float2ToImVec2(windowOffset));
        ImGui::PopStyleColor();
        ImGui::PopClipRect();
        m_uiElements.clear();
    }
}