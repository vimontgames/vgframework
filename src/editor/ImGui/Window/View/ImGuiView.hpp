#include "ImguiView.h"
#include "gfx/ITexture.h"
#include "renderer/IImGuiAdapter.h"
#include "renderer/IPicking.h"
#include "gfx/ITexture.h"
#include "core/IInput.h"
#include "editor/Options/EditorOptions.h"
#include "editor/ImGui/Extensions/ImGuizmo/ImGuizmoAdapter.h"

#include "Shaders/system/picking.hlsli"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiView::ImGuiView(const char * _icon, const core::string & _path, const string & _name, Flags _flags, gfx::ViewTarget _target, uint _index) :
        ImGuiWindow(_icon, _path, _name, _flags),
        m_target(_target),
        m_index(_index)
    {

    }

    //--------------------------------------------------------------------------------------
    ImGuiView::~ImGuiView()
    {
        VG_SAFE_RELEASE(m_texture);
        if (m_view)
        {
            auto viewID = m_view->GetViewID();
            VG_SAFE_RELEASE(m_view);
            Editor::get()->getRenderer()->RemoveView(viewID);
        }
    }

    //--------------------------------------------------------------------------------------
    ImGuiView::EditorCamera::EditorCamera() :
        m_matrix(core::float4x4::identity()),
        m_fovY(core::PI / 4.0f),
        m_near(0.01f),
        m_far(1024.0f),
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
    //void ImGuiView::setName(const string & _name)
    //{
    //    ImGuiWindow::setName(_name);
    //    if (m_view)
    //        m_view->setName(_name);
    //}

    //--------------------------------------------------------------------------------------
    void ImGuiView::updateEditorCamera(float _dt)
    {
        auto view = m_view;
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

            if (view->IsActive() && view->IsMouseOverView())
            {
                IInput * input = Kernel::getInput();
                
                const bool alt = input->IsKeyPressed(Key::LALT);

                if (alt)
                {
                    if (m_view->GetPickingHitCount() > 0)
                    {
                        // get closest hit
                        const PickingHit & pickingHit = m_view->GetPickingHit(0);
                        zoomDir = normalize(T.xyz - pickingHit.m_pos.xyz);
                    }
                }
                
                const int3 delta = input->GetMouseDelta();

                if (input->IsKeyPressed(Key::LSHIFT))
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
                    if (!input->IsKeyPressed(Key::LCONTROL))
                    {
                        if (input->IsKeyPressed(Key::A))
                            T -= moveSpeed * I;
                        else if (input->IsKeyPressed(Key::D))
                            T += moveSpeed * I;

                        if (input->IsKeyPressed(Key::PAGEDOWN))
                            T -= moveSpeed * J;
                        else if (input->IsKeyPressed(Key::PAGEUP))
                            T += moveSpeed * J;
                    }
                }

                // zoom
                if (delta.z != 0)
                {
                    //VG_DEBUGPRINT("[EditorCam] Zoom %i\n", (int)delta.z);
                    T.xyz = T.xyz - (float)(delta.z) * zoomSpeed * zoomDir;
                }
                else
                {
                    // zoom using keyboard
                    if (!input->IsKeyPressed(Key::LCONTROL))
                    {
                        if (input->IsKeyPressed(Key::W))
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
                    
                    const float2 viewSize = m_view->GetSize();
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

            view->SetupPerspectiveCamera(editorCam.m_matrix, float2(editorCam.m_near, editorCam.m_far), editorCam.m_fovY);
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::IView::Flags ImGuiView::GetViewFlags() const
    {
        return gfx::IView::Flags::Picking;
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
    void ImGuiView::DrawGUI()
    {
        // TODO : update editor camera *BEFORE* render?
        if (m_target == gfx::ViewTarget::Editor)
            updateEditorCamera(getEngine()->GetTime().m_dt);

        auto * renderer = Editor::get()->getRenderer();

        ImGui::SetNextWindowSizeConstraints(ImVec2(640, 480), ImVec2(MAX_FLOAT, MAX_FLOAT));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
        
        // By default, Windows are uniquely identified by their title.
        // You can use the "##" and "###" markers to manipulate the display/ID.
        // Using "##" to display same title but have unique identifier.
        // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
        // https://skia.googlesource.com/external/github.com/ocornut/imgui/+/refs/tags/v1.73/imgui_demo.cpp

        const string & name = getIconizedName();
        string title;
        
        if (any(m_size > 0) && m_view && m_view->IsVisible())
            title = fmt::sprintf("%s %ux%u###%s", name, (uint)m_size.x, (uint)m_size.y, name);
        else
            title = fmt::sprintf("%s###%s", name, name);

        if (ImGui::Begin(title.c_str(), &m_isVisible, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs))
        {
            // Compute Window content size
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            //VG_ASSERT(vMax.x > vMin.x && vMax.y > vMin.y, "vMin = (%f,%f) vMax = (%f, %f)", vMin.x, vMin.y, vMax.x, vMax.y);

            m_size = uint2((uint)max(vMax.x - vMin.x, 1.0f), (uint)max(vMax.y-vMin.y, 1.0f));

            bool draw = true;

            if (!m_view)
            {
                // get free index
                if (m_index == -1)
                {
                    auto * renderer = Editor::get()->getRenderer();
                    auto views = renderer->GetViews(m_target);
                    for (uint i = 0; i < views.size(); ++i)
                    {
                        if (!views[i])
                            m_index = i;
                    }
                    if (m_index == -1)
                        m_index = (uint)views.size();
                }

                if (gfx::IView * view = renderer->GetView(gfx::ViewID(m_target, m_index)))
                {
                    m_view = view;                    
                    VG_SAFE_INCREASE_REFCOUNT(view);
                }
                else
                {
                    // Create or update view using IRenderer
                    gfx::CreateViewParams params;
                    params.size = m_size;
                    params.world = GetWorld();
                    params.target = m_target;
                    params.dest = nullptr;    // No RenderTarget yet

                    string viewName = asString(params.target) + "View";

                    // Create a view with picking for editor views
                    gfx::IView::Flags viewFlags = GetViewFlags();

                    m_view = renderer->CreateView(params, viewName, viewFlags);
                    renderer->AddView(m_view);
                    m_view->setName(getName());
                }
                draw = false;
            }

            m_view->SetSize(m_size);

            auto texture = m_texture;

            if (!m_texture || m_texture->GetWidth() != m_size.x || m_texture->GetHeight() != m_size.y)
            {
                // As we're executing framegraph we can't fully release the texture right now because it may be still in use
                if (m_texture)
                    renderer->ReleaseAsync(m_texture);

                gfx::TextureDesc texDesc = gfx::TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm_sRGB, TextureFlags::RenderTarget, m_size.x, m_size.y);
                const auto & viewID = m_view->GetViewID();
                string texName = m_view->GetFrameGraphID("Dest");
                m_texture = renderer->CreateTexture(texDesc, texName);
                m_view->SetSize(m_size);
                m_view->SetRenderTarget(m_texture);

                // Tell the Renderer we resized stuff to clean up no more used buffers
                renderer->SetResized();

                draw = false;
            }

            // Set mouse offset
            ImVec2 mouseOffset = ImGui::GetCursorScreenPos();
            m_view->SetMouseOffset(uint2(mouseOffset.x, mouseOffset.y));

            if (texture)
            {
                auto * imGuiAdapter = renderer->GetImGuiAdapter();
                ImTextureID texID = imGuiAdapter->GetTextureID(texture);
                ImGui::Image(texID, ImVec2((float)m_size.x, (float)m_size.y));

                if (ImGui::IsWindowFocused())
                    m_view->SetActive(true);
                else
                    m_view->SetActive(false);

                imGuiAdapter->ReleaseTextureID(texID);
            }

            const auto options = EditorOptions::get();
            bool showStats = options->showCullingInfo();
            if (m_view && showStats)
            {
                const auto & stats = m_view->GetViewCullingStats();
                uint line = 0;

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetWindowContentRegionMin().y + style::font::Height * line++));
                ImGui::Text("Opaque %u", stats.opaque);

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetWindowContentRegionMin().y + style::font::Height * line++) );
                ImGui::Text("Transparent %u", stats.transparent);

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetWindowContentRegionMin().y + style::font::Height * line++) );
                ImGui::Text("Directional %u", stats.directional);

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetWindowContentRegionMin().y + style::font::Height * line++));
                ImGui::Text("Omni %u", stats.omni);

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetWindowContentRegionMin().y + style::font::Height * line++));
                ImGui::Text("Spot %u", stats.spot);
            }

            // Draw Border
            //auto * window = ImGui::FindWindowByName(name.c_str());
            //ImGuiDockNode * node = window->DockNode;
            //if (!node)
            //{
            //    auto borderColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            //    borderColor.w *= 0.5f;
            //    ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, GetColorU32(borderColor));
            //}

            bool activeGizmo = false;

            if (m_view && m_view->IsActive())
            {
                if (!drawGizmo())
                {
                    // Update picking if not currently manipulating gizmos
                    auto * renderer = Editor::get()->getRenderer();
                    auto picking = renderer->GetPicking();
                    bool showTooltip = Kernel::getInput()->IsKeyPressed(Key::LSHIFT); // ImGui::IsKeyPressed(ImGuiKey_LeftShift);
                    picking->Update(m_view, showTooltip, m_pickingTooltip);
                    if (!showTooltip)
                        m_pickingTooltip.clear();

                    if (!m_pickingTooltip.empty())
                        ImGui::SetTooltip(m_pickingTooltip.c_str());
                }
            }

            m_view->SetVisible(true);
        }
        else
        {
            if (m_view)
            {
                m_view->SetActive(false);
                m_view->SetVisible(false);
            }
        }

        if (!m_isVisible)
        {
            if (m_view)
            {
                m_view->SetActive(false);
                m_view->SetVisible(false);
            }
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::End();

        if (!m_isVisible)
            OnCloseWindow();
    }

    //--------------------------------------------------------------------------------------
    // Returns 'true' if any gizmo is manipulated (and thus we shoudn't update picking selection
    //--------------------------------------------------------------------------------------
    bool ImGuiView::drawGizmo()
    {
        ISelection * selection = Editor::get()->getSelection();
        const auto selectedObjectsNoFilter = selection->GetSelectedObjects();

        // filter selected objects by world to keep only those belonging to current view
        core::vector<core::IObject *> selectedObjects;
        const auto viewWorld = m_view->GetWorld();
        for (auto selected : selectedObjectsNoFilter)
        {
            auto go = dynamic_cast<IGameObject *>(selected);
            if (go && go->GetWorld() == viewWorld)
                selectedObjects.push_back(go);
        }

        if (m_view && m_view->IsToolmode() && selectedObjects.size() > 0)
        {
            const auto options = EditorOptions::get();
            const GizmoOptions & gizmoOptions = options->getGizmoOptions();

            const float * snap = nullptr;

            ImGuizmo::OPERATION imGuizmoOperation;
            switch (gizmoOptions.m_type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(gizmoOptions.m_type);

                case GizmoType::Translate:
                    imGuizmoOperation = ImGuizmo::TRANSLATE;
                    if (gizmoOptions.m_snapTranslate)
                        snap = (float*) &gizmoOptions.m_translationSnap;
                    break;

                case GizmoType::Rotate:
                    imGuizmoOperation = ImGuizmo::ROTATE;
                    if (gizmoOptions.m_snapRotate)
                        snap = &gizmoOptions.m_rotationSnapInDegrees;
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

            // get window and viewport coords
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            // set viewport size for Gizmo rendering
            ImGuizmo::SetRect(pos.x + vMin.x, pos.y + vMin.y, (float)m_size.x, (float)m_size.y);

            // clip gizmo rendering to viewport
            ImGui::PushClipRect(ImVec2(pos.x + vMin.x, pos.y + vMin.y), ImVec2(pos.x + vMin.x + m_size.x, pos.y + vMin.y + m_size.y), false);
            {
                const float * viewMatrix = (float *)&m_view->GetViewInvMatrix();
                const float * projMatrix = (float *)&m_view->GetProjectionMatrix();
                
                float4x4 & matrix = selection->GetSelectionMatrix();
                float4x4 delta = float4x4::identity();

                ImGuizmo::SetID((int)m_view->GetViewID().id);
                if (ImGuizmo::Manipulate(viewMatrix, projMatrix, imGuizmoOperation, imGuizmoSpace, (float *)&matrix, (float *)&delta, snap))
                {
                    auto selectedObjectsWithoutParents = selection->RemoveChildGameObjectsWithParents(selectedObjects);
                    bool skip = false;
                    if (!m_manipulating && ImGuizmo::IsUsing())
                    {
                        m_manipulating = true;
                        VG_INFO("[Editor] Start manipulating Gizmo");

                        if (Kernel::getInput()->IsKeyPressed(Key::LSHIFT))
                        {
                            // Duplicate the current selection and manipulate the copy
                            VG_INFO("[Editor] Duplicate current selection");
                            matrix = core::float4x4::identity();
                            delta = core::float4x4::identity();
                            ImGuizmo::Manipulate(viewMatrix, projMatrix, imGuizmoOperation, imGuizmoSpace, (float *)&matrix, (float *)&delta, snap);
                            selection->SetSelectedObjects((core::vector<IObject*>&)selection->DuplicateGameObjects(selectedObjectsWithoutParents));
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
                            mat = mul(mat, delta);
                            go->SetGlobalMatrix(mat);
                        }
                    }
                }

                if (m_manipulating && !ImGuizmo::IsUsing())
                {
                    VG_INFO("[Editor] Stop manipulating Gizmo");
                    m_manipulating = false;
                }
            }
            ImGui::PopClipRect();

            return ImGuizmo::IsOver();
        }

        return false;
    }
}