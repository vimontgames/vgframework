#include "ImguiView.h"
#include "core/IInput.h"
#include "gfx/ITexture.h"
#include "renderer/IUIRenderer.h"
#include "renderer/IImGuiAdapter.h"
#include "renderer/IPicking.h"
#include "renderer/ICameraLens.h"
#include "renderer/ICameraSettings.h"
#include "engine/ISnapComponent.h"
#include "editor/Options/EditorOptions.h"
#include "editor/ImGui/Extensions/ImGuizmo/ImGuizmoAdapter.h"

#include "Shaders/system/picking.hlsli"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiView::ImGuiView(const char * _icon, const core::string & _path, const string & _name, Flags _flags, gfx::ViewportTarget _target, uint _viewportIndex) :
        ImGuiWindow(_icon, _path, _name, _flags),
        m_target(_target),
        m_viewportIndex(_viewportIndex)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiView::~ImGuiView()
    {
        VG_SAFE_RELEASE(m_texture);
        VG_SAFE_RELEASE(m_viewport);
    }

    //--------------------------------------------------------------------------------------
    ImGuiView::EditorCamera::EditorCamera() :
        m_matrix(core::float4x4::identity()),
        m_moveSpeed(1.0f),
        m_rotSpeed(1.0f),
        m_panXYSpeed(1.0f),
        m_zoomSpeed(1.0f),
        m_panning(false)
    {
        m_matrix[0] = float4(1, 0, 0, 0);
        m_matrix[1] = float4(0, 0, 1, 0);
        m_matrix[2] = float4(0, -1, 0, 0);
        m_matrix[3] = float4(0, -8, 2, 1);
    }

    //--------------------------------------------------------------------------------------
    ImGuiView::EditorCamera::~EditorCamera()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::updateEditorCamera(float _dt)
    {
        auto * renderer = Editor::get()->getRenderer();

        if (!m_viewport)
            return;

        const auto & viewIDs = m_viewport->GetViewIDs();
       
        auto it = viewIDs.find(0);
        if (it == viewIDs.end())
            return;

        auto view = renderer->GetView(it->second);
        if (view)
        {
            auto & editorCam = m_editorCam;

            float mouseSpeedX = editorCam.m_rotSpeed;
            float mouseSpeedY = editorCam.m_rotSpeed;
            float moveSpeed = editorCam.m_moveSpeed * _dt;
            float panXYSpeed = editorCam.m_panXYSpeed * _dt;
            float zoomSpeed = editorCam.m_zoomSpeed * _dt;

            float4 I = editorCam.m_matrix[0];
            float4 J = editorCam.m_matrix[1];
            float4 K = editorCam.m_matrix[2];
            float4 T = editorCam.m_matrix[3];

            float3 zoomDir = normalize(K.xyz);

            if (view->IsFocused() && view->IsMouseOverView())
            {
                IInput * input = Kernel::getInput();
                
                const bool ctrl = input->IsKeyPressed(Key::LeftControl) || input->IsKeyPressed(Key::RightControl);
                const bool alt = input->IsKeyPressed(Key::LeftAlt) || input->IsKeyPressed(Key::RightAlt);
                const bool shift = input->IsKeyPressed(Key::LeftShift) || input->IsKeyPressed(Key::RightShift);

                if (!ctrl && !shift)
                {
                    auto options = EditorOptions::get();

                    if (input->IsKeyJustPressed(Key::T))
                        options->setGizmoType(GizmoType::Translate);
                    else if (input->IsKeyJustPressed(Key::R))
                        options->setGizmoType(GizmoType::Rotate);
                    else if (input->IsKeyJustPressed(Key::S))
                        options->setGizmoType(GizmoType::Scale);
                    else if (input->IsKeyJustPressed(Key::A))
                        options->setSnap(!options->getSnap());
                }

                if (alt)
                {
                    if (view->GetPickingHitCount() > 0)
                    {
                        // get closest hit
                        const PickingHit & pickingHit = view->GetPickingHit(0);
                        zoomDir = normalize(T.xyz - pickingHit.m_pos.xyz);
                    }
                }
                
                const int3 delta = input->GetMouseDelta();

                if (shift)
                    moveSpeed *= 16.0f;

                if (input->IsMouseButtonPressed(MouseButton::Middle))
                {
                    if (!editorCam.m_panning)
                    {
                        // Start pan
                        editorCam.m_panning = true;
                        //VG_DEBUGPRINT("[EditorCam] Start Pan\n");
                    }
                    else
                    {
                        // Continue pan
                        //VG_DEBUGPRINT("[EditorCam] Pan %i %i\n", (int)delta.x, (int)delta.y);

                        if (any(delta.xy != 0))
                        {
                            T.xyz = T.xyz - (float)(delta.x) * panXYSpeed * I.xyz;
                            T.xyz = T.xyz + (float)(delta.y) * panXYSpeed * J.xyz;
                            
                        }
                    }
                }
                else 
                {
                    if (editorCam.m_panning)
                    {
                        // Stop pan
                        editorCam.m_panning = false;
                        //VG_DEBUGPRINT("[EditorCam] Stop Pan\n");
                    }

                    // Pan using keyboard
                    if (!input->IsKeyPressed(Key::LeftControl))
                    {
                        if (input->IsKeyPressed(Key::Q))
                            T -= moveSpeed * I;
                        else if (input->IsKeyPressed(Key::D))
                            T += moveSpeed * I;

                        if (input->IsKeyPressed(Key::PageDown))
                            T -= moveSpeed * J;
                        else if (input->IsKeyPressed(Key::PageUp))
                            T += moveSpeed * J;
                    }
                }

                // zoom
                if (!ctrl && delta.z != 0)
                {
                    //VG_DEBUGPRINT("[EditorCam] Zoom %i\n", (int)delta.z);
                    T.xyz = T.xyz - (float)(delta.z) * zoomSpeed * zoomDir;
                }
                else
                {
                    // zoom using keyboard
                    if (!ctrl)
                    {
                        if (input->IsKeyPressed(Key::Z))
                            T -= moveSpeed * K;
                        else if (input->IsKeyPressed(Key::S))
                            T += moveSpeed * K;
                    }
                }

                editorCam.m_matrix = float4x4(I, J, K, T);

                if (input->IsMouseButtonPressed(MouseButton::Right) && any(delta.xy != 0))
                {
                    float3 position = T.xyz;

                    float3 pivot = float3(0, 0, 0);
                    ISelection * selection = Editor::get()->getSelection();

                    bool rotateAroundPivot= false;

                    if (alt && selection->GetSelectedObjects().size() > 0)
                    {
                        pivot = selection->GetSelectionMatrix()[3].xyz;
                        rotateAroundPivot = true;
                    }

                    float3 viewDir = (position - pivot);
                    
                    const float2 viewSize = view->GetSize();
                    float deltaAngleX = (2.0f * PI / viewSize.x * mouseSpeedX);
                    float deltaAngleY = (PI / viewSize.y * mouseSpeedY);
                    float xAngle = (float)delta.x * deltaAngleX;
                    float yAngle = (float)delta.y * deltaAngleY;
                    
                    float3 up = float3(0, 0, 1);

                    if (rotateAroundPivot)
                    {
                        float4x4 rotationMatrixX = float4x4::rotation_axis(up, xAngle);
                        editorCam.m_matrix[3].xyz -= pivot;
                        editorCam.m_matrix = mul(editorCam.m_matrix, rotationMatrixX);

                        float4x4 rotationMatrixY = float4x4::rotation_axis(editorCam.m_matrix[0].xyz, yAngle);
                        editorCam.m_matrix = mul(editorCam.m_matrix, rotationMatrixY);
                        editorCam.m_matrix[3].xyz += pivot;
                    }
                    else
                    {
                        float4x4 rotationMatrixX = float4x4::rotation_axis(up, -xAngle);

                        T = editorCam.m_matrix[3];
                        editorCam.m_matrix[3] = float4(0, 0, 0, 1);
                        editorCam.m_matrix = mul(editorCam.m_matrix, rotationMatrixX);

                        float4x4 rotationMatrixY = float4x4::rotation_axis(editorCam.m_matrix[0].xyz, -yAngle);
                        editorCam.m_matrix = mul(editorCam.m_matrix, rotationMatrixY);
                        editorCam.m_matrix[3] = T;
                    }
                }
            }

            const auto * editorCameraSettings = EditorOptions::get()->GetCameraSettings();
            view->SetupPhysicalCamera(editorCam.m_matrix, editorCameraSettings);
        }
    }

    //--------------------------------------------------------------------------------------
    renderer::ViewFlags ImGuiView::GetViewFlags() const
    {
        return (renderer::ViewFlags)0x0;
    }

    //--------------------------------------------------------------------------------------
    IWorld * ImGuiView::GetWorld() const
    {
        return Editor::get()->getEngine()->GetMainWorld();
    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiView::OnCloseWindow()
    {
       
    }

    //--------------------------------------------------------------------------------------
    core::string ImGuiView::GetTitle() const
    {
        string title;

        const string & name = getIconizedName();

        //if (all(m_size > 0))
        //    title = fmt::sprintf("%s %ux%u###%s", name, (uint)m_size.x, (uint)m_size.y, name);
        //else
            title = fmt::sprintf("%s###%s", name, name);

        return title;
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewportTarget ImGuiView::getViewportTarget() const
    { 
        return m_target; 
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewIndex ImGuiView::getViewportIndex() const
    { 
        return m_viewport ? m_viewport->GetViewportID().index : -1; 
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::DrawGUI()
    {
        float padding = 1;

        // TODO : update editor camera *BEFORE* render?
        if (m_target == gfx::ViewportTarget::Editor)
            updateEditorCamera(getEngine()->GetTime().m_realDeltaTime);

        // Update
        {
            auto * editor = Editor::get();
            auto * selection = editor->getSelection();
            auto selectedObjects = selection->GetSelectedObjects();
            auto topLevelGameObjects = selection->RemoveChildGameObjectsWithParents(selectedObjects);

            if (!ImGui::IsAnyItemActive())
            {
                if (ImGui::IsKeyPressed(ImGuiKey_F) && topLevelGameObjects.size() > 0)
                {
                    editor->focus(topLevelGameObjects);
                }
                else if (ImGui::IsKeyPressed(ImGuiKey_Delete) && topLevelGameObjects.size() > 0)
                {
                    editor->deleteGameObjects(topLevelGameObjects);
                }
            }
        }

        auto * renderer = Editor::get()->getRenderer();

        ImGuiStyle & style = ImGui::GetStyle();
        const float saveWindowRounding = style.WindowRounding;
        style.WindowRounding = 0; 

        const auto titleBarHeight = editor::style::font::DefaultFontHeight + 2 * ImGui::GetStyle().FramePadding.y;

        ImGui::SetNextWindowSizeConstraints(ImVec2(320, 256 + titleBarHeight), ImVec2(MAX_FLOAT, MAX_FLOAT));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
        
        // By default, Windows are uniquely identified by their title.
        // You can use the "##" and "###" markers to manipulate the display/ID.
        // Using "##" to display same title but have unique identifier.
        // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
        // https://skia.googlesource.com/external/github.com/ocornut/imgui/+/refs/tags/v1.73/imgui_demo.cpp

        string title = GetTitle();        

        if (m_closeNextFrame)
        {
            m_isVisible = false;
            m_closeNextFrame = false;
        }

        const bool active = m_viewport && m_viewport->AnyFocused();
        if (active)
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_TabSelected));

        extern const char * g_previousFocusedWindowName;
        if (g_previousFocusedWindowName && !strcmp(g_previousFocusedWindowName, title.c_str()))
        {
            static int counter = 0;
            if (counter == 1)
            {
                SetNextWindowFocus();
                g_previousFocusedWindowName = nullptr;
                counter = 0;
            }
            else
                counter++;
        }

        if (ImGui::Begin(title.c_str(), &m_isVisible, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar))
        {
            // Menu
            if (ShowTitlebarMenu())
            {
                ImGui::PopStyleVar();
                DrawTitlebarMenu();
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
            }

            // Toolbar
            bool hasToolbar = ShowToolbar();
            float posY = ImGui::GetCursorPosY();
            if (hasToolbar)
            {
                ImGui::PopStyleVar();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                DrawToolbar();
                ImGui::PopStyleVar();

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
            }
            float toolbarHeight = ImGui::GetCursorPosY() - posY;

            const auto windowPos = ImGui::GetWindowPos();

            // Compute Window content size
            auto * curWin = ImGui::GetCurrentWindow();
            const ImVec2 vMin = curWin->InnerRect.Min + ImVec2(1, 1);
            const ImVec2 vMax = curWin->InnerRect.Max - ImVec2(1,1);
            const ImVec2 windowRegionMin = curWin->InnerRect.Min - curWin->Pos;
            const ImVec2 windowRegionMax = curWin->InnerRect.Max - curWin->Pos;

            //VG_ASSERT(vMax.x > vMin.x && vMax.y > vMin.y, "vMin = (%f,%f) vMax = (%f, %f)", vMin.x, vMin.y, vMax.x, vMax.y);

            m_size = uint2((uint)max(vMax.x - vMin.x, 1.0f), (uint)max(vMax.y-vMin.y, 1.0f));
            m_size.y = (uint)max(0, (int)m_size.y- (int)toolbarHeight);

            m_size.xy = max(m_size.xy, uint2(1, 1));

            // Create viewport if needed
            if (!m_viewport)
            {
                // create viewport or use existing
                if ((gfx::ViewportIndex)-1 == m_viewportIndex)
                    m_viewportIndex = renderer->GetFreeViewIndex((gfx::ViewTarget)m_target);

                if (auto * viewport = renderer->GetViewport(gfx::ViewportID(m_target, m_viewportIndex)))
                {
                    m_viewport = viewport;
                    VG_SAFE_INCREASE_REFCOUNT(viewport);
                }
                else
                {
                    renderer::CreateViewportParams viewportParams(m_target, m_size);
                    string vpName = asString(viewportParams.target) + "Viewport";
                    m_viewport = renderer->CreateViewport(viewportParams, vpName);
                    renderer->AddViewport(m_viewport);
                }
            }

            auto & viewIDs = m_viewport->GetViewIDs();

            // Add default view if needed but for editor view only ('Game' views needs to be created explicitely)
            if (m_target == ViewportTarget::Editor && viewIDs.size() == 0)
            {
                renderer::CreateViewParams viewParams;
                                           viewParams.size = m_size;
                                           viewParams.world = GetWorld();
                                           viewParams.target = (gfx::ViewTarget)m_target;  // TODO: hazardous cast between ViewTarget and ViewportTarget? Create single enum?
                                           viewParams.dest = nullptr;                      // No RenderTarget yet
                                           viewParams.viewport = m_viewport;
            
                string viewName = asString(viewParams.target) + "View";
            
                // Create a view
                renderer::ViewFlags viewFlags = GetViewFlags();
            
                if (auto * view = renderer->CreateView(viewParams, viewName, viewFlags))
                {
                    renderer->AddView(view);
                    m_viewport->AddView(0, view->GetViewID());
                    VG_SAFE_RELEASE(view);
                }
            }

            auto * world = GetWorld();
            for (const auto & pair : viewIDs)
            {
                if (auto * view = renderer->GetView(pair.second))
                    view->SetWorld(world);
            }

            if (!UpdateScene())
                m_closeNextFrame = true;

            // Recompute texture size from viewport offset & scale
            if (m_viewport)
            {
                m_viewport->SetRenderTargetSize(m_size);
                if (m_texture)
                    m_viewport->SetRenderTarget(m_texture);
            }

            // Hack: Do not use texture on first frame because it may have just become visible but the view not yet renderer (alt fix: last rendered frame counter in View?)
            auto texture = (ImGui::IsWindowAppearing() || !m_viewport->AnyRender()) ? nullptr :  m_texture;

            auto fmt = HDR::None != renderer->GetHDR() ? PixelFormat::R16G16B16A16_float : PixelFormat::R8G8B8A8_unorm_sRGB;

            // Create destination texture if it does not exist or is not matching
            if (world && m_viewport && m_viewport->GetViewIDs().size() > 0 && (!m_texture || m_texture->GetWidth() != m_size.x || m_texture->GetHeight() != m_size.y || fmt != m_texture->GetPixelFormat()))
            {
                // As we're executing framegraph we can't fully release the texture right now because it may be still in use
                if (m_texture)
                    renderer->ReleaseAsync(m_texture);

                gfx::TextureDesc texDesc = gfx::TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, fmt, TextureFlags::RenderTarget, m_size.x, m_size.y);
                const auto & viewportID = m_viewport->GetViewportID();
                string texName = m_viewport->GetFrameGraphID("Dest");
                m_texture = renderer->CreateTexture(texDesc, texName);
                m_viewport->SetRenderTargetSize(m_size);
                m_viewport->SetRenderTarget(m_texture);

                // Tell the Renderer we resized stuff to clean up no more used buffers
                renderer->SetResized();
            }
            else if (!world)
            {
                if (m_viewport)
                    m_viewport->SetRenderTarget(nullptr);

                texture = nullptr;
            }
            
            // Set mouse offset
            if (m_viewport)
            {
                ImVec2 mouseOffset = ImGui::GetCursorScreenPos();

                for (const auto & pair : viewIDs)
                {
                    if (auto * view = renderer->GetView(pair.second))
                        view->SetMouseOffset(uint2(mouseOffset.x, mouseOffset.y));
                }
            }
            
            auto pos = ImGui::GetCursorPos();
            auto * window = ImGui::FindWindowByName(title.c_str());
            ImGuiDockNode * node = window->DockNode;
            const bool is_drag_docking = GImGui->MovingWindow == window;
            const float alpha = is_drag_docking ? 0.5f : 1.0f;
            auto * imGuiAdapter = renderer->GetImGuiAdapter();

            if (texture)
            {
                // TODO: Vulkan we have to hide the texture for one frame when view is closed/reopened? because it happens during ImGui render? SetVisibleAsync?
                ImTextureID texID = imGuiAdapter->GetTextureID(texture);
                ImGui::ImageWithBg(texID, ImVec2((float)m_size.x, (float)m_size.y), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, alpha));
            }
            else
            {
                ImDrawList * draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRectFilled(windowPos + windowRegionMin, windowPos + windowRegionMin + ImVec2((float)m_size.x, (float)m_size.y), 0xFF000000);
            }

            if (ShowContextMenu())
                DrawContextMenu();              
            
            if (ImGui::IsWindowFocused())
                m_viewport->SetFocused(true);
            else
                m_viewport->SetFocused(false);

            if (texture)
                imGuiAdapter->ReleaseTextureID(texture);

            if (m_viewport)
            {
                ImVec2 mouseOffset = ImGui::GetCursorScreenPos();

                for (const auto & pair : viewIDs)
                {
                    if (auto * view = renderer->GetView(pair.second))
                    {
                        if (auto * uiRenderer = view->GetUIRenderer())
                        {
                            ImGui::SetCursorPos(ImVec2(0, titleBarHeight + toolbarHeight));
                            uiRenderer->RenderWindowed(renderer::RenderUIType::RenderUIType_3D);

                            const auto options = EditorOptions::get();
                            bool debugCulling = options->IsDebugCulling();

                            const ImVec2 rectPos = ImGui::GetCurrentWindow()->InnerRect.Min + ImVec2(1,1);

                            const auto rectColor = 0x7F000000;
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));

                            // TODO: move debugCulling flag and implementation to renderer options and implement in renderer view code 
                            // (previously ImGui was only available in editor but now it's also available in renderer)
                            if (view && debugCulling)
                            {
                                const auto & stats = view->GetViewCullingStats();
                                uint line = 0;

                                ImVec2 pos = ImVec2((float)view->GetOffset().x, (float)view->GetOffset().y);

                                // Instances
                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                string text = fmt::sprintf("Opaque %u", stats.opaque);
                                ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("AlphaTest %u", stats.alphatest);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Transparent %u", stats.transparent);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Particles %u", stats.particleSystem);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Decal %u", stats.decal);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Outline %u", stats.outline);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                // Lights
                                pos.y += style::font::DefaultFontHeight;

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Directional %u", stats.directional);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Omni %u", stats.omni);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());

                                ImGui::SameLine();
                                ImGui::SetCursorPos(window->ContentRegionRect.Min - window->Pos + pos);
                                text = fmt::sprintf("Spot %u", stats.spot);
                                textSize = ImGui::CalcTextSize(text.c_str());
                                ImGui::GetWindowDrawList()->AddRectFilled(rectPos + pos, rectPos + pos + textSize, rectColor);
                                pos.y += style::font::DefaultFontHeight;
                                ImGui::Text(text.c_str());
                            }

                            ImGui::PopStyleColor();
                        }
                    }
                }

                if (auto * uiRenderer = m_viewport->GetUIRenderer())
                {
                    ImGui::SetCursorPos(ImVec2(0, titleBarHeight + toolbarHeight));
                    uiRenderer->RenderWindowed(renderer::RenderUIType::RenderUIType_2D);
                }
            }

            bool activeGizmo = false;

            if (m_viewport)
            {
                ImVec2 mouseOffset = ImGui::GetCursorScreenPos();

                for (const auto & pair : viewIDs)
                {
                    if (auto * view = renderer->GetView(pair.second))
                    {
                        if (!drawGizmo(view) && view->IsFocused())
                        {
                            // Update picking if not currently manipulating gizmos
                            auto * renderer = Editor::get()->getRenderer();
                            auto picking = renderer->GetPicking();
                            bool showTooltip = Kernel::getInput()->IsKeyPressed(Key::LeftShift); // ImGui::IsKeyPressed(ImGuiKey_LeftShift);

                            string tooltip, tooltipDbg;
                            picking->Update(view, showTooltip, tooltip, tooltipDbg);

                            if (EditorOptions::get()->IsDebugPicking())
                                tooltip += tooltipDbg;

                            if (!tooltip.empty())
                                ImGui::SetTooltip(tooltip.c_str());
                        }
                    }
                }

                m_viewport->SetVisible(true);
            }
        }
        else
        {
            if (m_viewport)
            {
                m_viewport->SetFocused(false);
                m_viewport->SetVisible(false);
            }
        }

        if (!m_isVisible)
        {
            if (m_viewport)
            {
                m_viewport->SetFocused(false);
                m_viewport->SetVisible(false);
            }
        }

        if (active)
            ImGui::PopStyleColor();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::End();

        style.WindowRounding = saveWindowRounding;

        if (!m_isVisible)
            OnCloseWindow();
    }

    //--------------------------------------------------------------------------------------
    void findMaximumSnap(const vector<IObject *> & _objects, float3 & _translationSnap, float & _rotationSnapInDegrees, float & _scaleSnap)
    {
        VG_PROFILE_CPU("findMaximumSnap");

        #if 1

        // Recursive version
        for (uint i = 0; i < _objects.size(); ++i)
        {
            const IGameObject * go = (IGameObject*)_objects[i];

            if (const auto * snap = go->GetComponentT<engine::ISnapComponent>())
            {
                float3 trans;
                if (snap->TryGetSnapTranslate(trans))
                    _translationSnap = max(_translationSnap, trans);

                float rot;
                if (snap->TryGetSnapRotate(rot))
                    _rotationSnapInDegrees = max(_rotationSnapInDegrees, rot);

                float scale;
                if (snap->TryGetSnapScale(scale))
                    _scaleSnap = max(_scaleSnap, scale);
            }

            const auto & children = (const vector<IObject *> &)go->GetChildren();
            findMaximumSnap(children, _translationSnap, _rotationSnapInDegrees, _scaleSnap);
        }

        #else

        // Non-recursive version (slower?)
        for (uint i = 0; i < _objects.size(); ++i)
        {
            const IGameObject * go = (IGameObject *)_objects[i];

            auto snapComponents = go->GetComponentsInChildrenT<engine::ISnapComponent>();

            for (const auto & snap : snapComponents)
            {
                float3 trans;
                if (snap->TryGetSnapTranslate(trans))
                    _translationSnap = max(_translationSnap, trans);

                float rot;
                if (snap->TryGetSnapRotate(rot))
                    _rotationSnapInDegrees = max(_rotationSnapInDegrees, rot);

                float scale;
                if (snap->TryGetSnapScale(scale))
                    _scaleSnap = max(_scaleSnap, scale);
            }
        }

        #endif
    }

    //--------------------------------------------------------------------------------------
    // Returns 'true' if any gizmo is manipulated (and thus we shoudn't update picking selection)
    //--------------------------------------------------------------------------------------
    bool ImGuiView::drawGizmo(const renderer::IView * _view)
    {
        VG_PROFILE_CPU("drawGizmo");

        ISelection * selection = Editor::get()->getSelection();
        const auto & selectedObjectsNoFilter = selection->GetSelectedObjects();

        // filter selected objects by world to keep only those belonging to current view
        core::vector<core::IObject *> selectedObjects;
        const auto viewWorld = _view->GetWorld();
        for (auto selected : selectedObjectsNoFilter)
        {
            auto go = dynamic_cast<IGameObject *>(selected);
            if (go && go->GetWorld() == viewWorld)
                selectedObjects.push_back(go);
        }

        if (_view && _view->IsToolmode() && selectedObjects.size() > 0)
        {
            const auto window = ImGui::GetCurrentWindow();
            const auto options = EditorOptions::get();
            const GizmoOptions & gizmoOptions = options->getGizmoOptions();

            const float * snap = nullptr;

            float3 translationSnap = gizmoOptions.m_translationSnap;
            float rotationSnapInDegrees = gizmoOptions.m_rotationSnapInDegrees;
            float scaleSnap = gizmoOptions.m_scaleSnap;

            if (selectedObjects.size() != m_selectedObjectCount)
            {
                if (gizmoOptions.m_snapTranslate || gizmoOptions.m_snapRotate || gizmoOptions.m_snapScale)
                {
                    // 'findMaximumSnap' is a very slow operation in debug so only do it when selection changes
                    m_maxTranslationSnap = float3(0, 0, 0);
                    m_maxRotationSnapInDegrees = 0.0f;
                    m_maxScaleSnap = 0.0f;
                    
                    findMaximumSnap(selectedObjects, m_maxTranslationSnap, m_maxRotationSnapInDegrees, m_maxScaleSnap);

                    m_selectedObjectCount = (uint)selectedObjects.size();
                }
            }

            if (gizmoOptions.m_snapTranslate)
                translationSnap = max(translationSnap, m_maxTranslationSnap);

            if (gizmoOptions.m_snapRotate)
                rotationSnapInDegrees = max(rotationSnapInDegrees, m_maxRotationSnapInDegrees);

            if (gizmoOptions.m_snapScale)
                scaleSnap = max(scaleSnap, m_maxScaleSnap);

            ImGuizmo::OPERATION imGuizmoOperation;
            switch (gizmoOptions.m_type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(gizmoOptions.m_type);

                case GizmoType::Translate:
                    imGuizmoOperation = ImGuizmo::TRANSLATE;
                    if (gizmoOptions.m_snapTranslate)
                        snap = (float*)&translationSnap;
                    break;

                case GizmoType::Rotate:
                    imGuizmoOperation = ImGuizmo::ROTATE;
                    if (gizmoOptions.m_snapRotate)
                        snap = &rotationSnapInDegrees;
                    break;

                case GizmoType::Scale:
                    imGuizmoOperation = ImGuizmo::SCALE;
                    if (gizmoOptions.m_snapScale)
                        snap = &gizmoOptions.m_scaleSnap;
                    break;
            }

            ImGuizmo::MODE imGuizmoSpace;
            switch (gizmoOptions.m_space)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(gizmoOptions.m_space);     

                case GizmoSpace::World:
                    imGuizmoSpace = ImGuizmo::WORLD;
                    break;

                case GizmoSpace::Local:
                    imGuizmoSpace = ImGuizmo::LOCAL;
                    break;
            }

            ImGuizmo::AllowAxisFlip(false);

            // force rendering gizmo in current window
            ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

            float2 offset = (float2)m_size * _view->GetViewportOffset();
            float2 size = (float2)m_size * _view->GetViewportScale();

            // get window and viewport coords
            ImVec2 pos = ImGui::GetWindowPos() + ImVec2(offset.x, offset.y);
            ImVec2 vMin = window->InnerRect.Min - pos + ImVec2(1,1);
         
            // set viewport size for Gizmo rendering
            ImGuizmo::SetRect(pos.x + vMin.x, pos.y + vMin.y, size.x, size.y);

            // clip gizmo rendering to viewport
            ImGui::PushClipRect(ImVec2(pos.x + vMin.x, pos.y + vMin.y), ImVec2(pos.x + vMin.x + size.x -1, pos.y + vMin.y + size.y-1), false);
            {
                const float * viewMatrix = (float *)&_view->GetViewMatrix();
                const float * projMatrix = (float *)&_view->GetProjectionMatrix();
                
                float4x4 & selectionMatrix = selection->GetSelectionMatrix();
                float4x4 matrix = selectionMatrix;
                float4x4 delta = float4x4::identity();

                auto * undoRedoManager = Kernel::getUndoRedoManager();

                ImGuizmo::SetID((int)_view->GetViewID().id);
                if (ImGuizmo::Manipulate(viewMatrix, projMatrix, imGuizmoOperation, imGuizmoSpace, (float *)&matrix, (float *)&delta, snap))
                {
                    auto selectedObjectsWithoutParents = selection->RemoveChildGameObjectsWithParents(selectedObjects);
                    bool skip = false;
                    if (!m_manipulating && ImGuizmo::IsUsing())
                    {
                        m_manipulating = true;
                        VG_INFO("[Editor] Start manipulating Gizmo");

                        UndoRedoTarget undoRedoTarget(this, nullptr);
                        VG_ASSERT(false == undoRedoManager->HasCurrentlyEditedEntry());
                        auto * undoRedoGroup = new UndoRedoEntryGroup(asString(gizmoOptions.m_type));
                        for (uint i = 0; i < selectedObjectsWithoutParents.size(); ++i)
                        {
                            auto * obj = selectedObjectsWithoutParents[i];
                            auto * prop = obj->GetClassDesc()->GetPropertyByName("m_local");
                            undoRedoGroup->AddSubEntry(new UndoRedoPropertyEntry(obj, prop));
                        }

                        undoRedoManager->BeforeChange(undoRedoGroup);
                        undoRedoManager->SetCurrentUndoRedoTarget(undoRedoTarget);                       

                        if (Kernel::getInput()->IsKeyPressed(Key::LeftShift))
                        {
                            // Duplicate the current selection and manipulate the copy
                            VG_INFO("[Editor] Duplicate current selection");
                            matrix = core::float4x4::identity();
                            delta = core::float4x4::identity();
                            ImGuizmo::Manipulate(viewMatrix, projMatrix, imGuizmoOperation, imGuizmoSpace, (float *)&matrix, (float *)&delta, snap);
                            auto duplicateGameObjects = selection->DuplicateGameObjects(selectedObjectsWithoutParents);
                            selection->SetSelectedObjects((core::vector<IObject*>&)duplicateGameObjects);
                            skip = true;
                        }
                    }

                    // apply delta to selected objects without parents
                    if (!skip)
                    {
                        for (uint i = 0; i < selectedObjectsWithoutParents.size(); ++i)
                        {
                            IGameObject * go = selectedObjectsWithoutParents[i];
                            float4x4 mat = go->GetGlobalMatrix();
                            
                            if (imGuizmoOperation == ImGuizmo::SCALE)
                            {
                                float3 T = mat[3].xyz;
                                mat[3].xyz = float3(0, 0, 0);
                                mat = mul(mat, delta);
                                mat[3].xyz = T;
                            }
                            else
                            {
                                mat = mul(mat, delta);
                            }

                            go->SetGlobalMatrix(mat);
                        }

                        selectionMatrix = matrix;
                    }
                }

                if (m_manipulating && !ImGuizmo::IsUsing())
                {
                    VG_INFO("[Editor] Stop manipulating Gizmo");
                    m_manipulating = false;

                    // Finalize Undo/Redo entry after editing
                    undoRedoManager->ClearCurrentUndoRedoTarget();
                    VG_ASSERT(undoRedoManager->HasCurrentlyEditedEntry());
                    undoRedoManager->AfterChange();
                }
            }
            ImGui::PopClipRect();

            return ImGuizmo::IsOver();
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::focus(IGameObject * _gameObject)
    {
        core::vector<core::IGameObject * > array;
        array.push_back(_gameObject);
        focus(array);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::focus(const core::vector<core::IGameObject * > & _gameObjects)
    {
        if (_gameObjects.size() > 0)
        {
            const float3 offset = normalize(m_editorCam.m_matrix[2].xyz);
            core::AABB aabb;
            aabb.reset();

            for (uint i = 0; i < _gameObjects.size(); ++i)
            {
                const IGameObject * gameObject = _gameObjects[i];
                const auto world = gameObject->getGlobalMatrix();

                core::AABB gameObjectAABB;
                if (gameObject->TryGetAABB(gameObjectAABB))
                {
                    gameObjectAABB = AABB::transform(gameObjectAABB, world);
                    aabb.grow(gameObjectAABB);
                }     
                else
                {
                    aabb.grow(world[3].xyz);
                }
            }      

            VG_ASSERT(aabb.isFinite());
            
            float3 center = aabb.center();
            m_editorCam.m_matrix[3].xyz = center + offset * aabb.radius() * PI;

            // Find the scene lists this object is displayed into
            vector<ImGuiWindow *> sceneLists = Editor::get()->GetWindows("ImGuiSceneList");
            for (uint i = 0; i < sceneLists.size(); ++i)
            {
                ImGuiSceneList * sceneList = (ImGuiSceneList*)sceneLists[i];
                sceneList->focus(_gameObjects);
            }

            setWindowFocus();
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::setWindowFocus()
    {
        ImGui::SetWindowFocus(GetTitle().c_str());
    }
}