#include "ImguiView.h"
#include "gfx/ITexture.h"
#include "renderer/IImGuiAdapter.h"
#include "renderer/IPicking.h"
#include "gfx/ITexture.h"
#include "core/IInput.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiView::ImGuiView(const char * _icon, const core::string & _path, const string & _name, Flags _flags, gfx::ViewTarget _target) :
        ImGuiWindow(_icon, _path, _name, _flags),
        m_target(_target)
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
    void ImGuiView::DrawGUI()
    {
        auto * renderer = Editor::get()->getRenderer();

        ImGui::SetNextWindowSizeConstraints(ImVec2(256, 256), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
        
        // By default, Windows are uniquely identified by their title.
        // You can use the "##" and "###" markers to manipulate the display/ID.
        // Using "##" to display same title but have unique identifier.
        // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
        // https://skia.googlesource.com/external/github.com/ocornut/imgui/+/refs/tags/v1.73/imgui_demo.cpp

        const string & name = getIconizedName();
        string title = name;
        
        //if (any(m_size > 0))
        //    title = name + " (" + to_string(m_size.x) + "x" + to_string(m_size.y) + ")###" + name;

        if (ImGui::Begin((title).c_str(), &m_isVisible, ImGuiWindowFlags_NoFocusOnAppearing))
        {
            //if (IsWindowAppearing())
            //{
            //    VG_INFO("[Editor] Window \"%s\" is appearing", title.c_str());
            //}

            // Compute Window content size
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            m_size = uint2(vMax.x - vMin.x, vMax.y-vMin.y);

            bool draw = true;

            if (!m_view)
            {
                // Create empty view using IRenderer
                gfx::CreateViewParams params;
                                      params.size = m_size;
                                      params.universe = Editor::get()->getEngine()->getCurrentUniverse(); // TODO: do better
                                      params.target = m_target;
                                      params.dest = nullptr;    // No RenderTarget yet
            
                string viewName = asString(params.target) + "View";

                // Create a view with picking for editor views
                gfx::IView::Flags viewFlags = (gfx::IView::Flags)0;
                if (params.target == ViewTarget::Editor)
                    viewFlags |= gfx::IView::Flags::Picking;

                m_view = renderer->CreateView(params, viewName, viewFlags);
                renderer->AddView(m_view);
                draw = false;
            }

            m_view->SetSize(m_size);

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

            // Set relative mouse pos
            //ImVec2 mouseOffset = ImGui::GetCursorScreenPos();
            //auto input = Kernel::getInput();
            //uint2 mousePos = input->getMousePos();
            //m_view->SetMousePos(uint2((uint)mousePos.x - mouseOffset.x, (uint)mousePos.y - mouseOffset.y));

            // Set mouse offset
            ImVec2 mouseOffset = ImGui::GetCursorScreenPos();
            m_view->SetMouseOffset(uint2(mouseOffset.x, mouseOffset.y));

            if (draw) 
            {
                auto * imGuiAdapter = renderer->GetImGuiAdapter();
                ImTextureID texID = imGuiAdapter->GetTextureID(m_texture);
                ImGui::Image(texID, ImVec2((float)m_size.x, (float)m_size.y));

                if (ImGui::IsWindowFocused())
                    m_view->SetActive(true);
                else
                    m_view->SetActive(false);

                imGuiAdapter->ReleaseTextureID(texID);
            }

            // Draw Border
            //ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(0, 255, 0, 255));

            // Update picking
            auto picking = renderer->GetPicking();

            bool showTooltip = Kernel::getInput()->IsKeyPressed(Key::LSHIFT); // ImGui::IsKeyPressed(ImGuiKey_LeftShift);
            string tooltipMsg;
            picking->Update(m_view, showTooltip, tooltipMsg);

            if (showTooltip && !tooltipMsg.empty())
                ImGui::SetTooltip(tooltipMsg.c_str());

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

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::End();
    }
}