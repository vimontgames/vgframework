#include "ImguiView.h"
#include "graphics/renderer/View/Forward/ForwardView.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImGuiView::ImGuiView(const string & _name, Flags _flags, driver::ViewType _viewType) :
        ImguiEditor(_name, _flags),
        m_viewType(_viewType)
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
            Renderer::get()->RemoveView(viewID);
        }
  
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::update(double _dt)
    {
 
    }

    //--------------------------------------------------------------------------------------
    void ToolbarUI()
    {
        const float toolbarSize = 50;

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 16));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = 0
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings
            ;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::Begin("TOOLBAR", NULL, window_flags);
        ImGui::PopStyleVar();

        ImGui::Button("Toolbar goes here", ImVec2(0, 37));

        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::display()
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(256, 256), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
        
        // By default, Windows are uniquely identified by their title.
        // You can use the "##" and "###" markers to manipulate the display/ID.
        // Using "##" to display same title but have unique identifier.
        // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
        // https://skia.googlesource.com/external/github.com/ocornut/imgui/+/refs/tags/v1.73/imgui_demo.cpp

        const string & name = getName();
        string title = name + " (" + to_string(m_size.x) + "x" + to_string(m_size.y) + ")###" + name;

        if (ImGui::Begin(title.c_str(), &m_isVisible, ImGuiWindowFlags_NoFocusOnAppearing))
        {
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
                // Create empty view
                driver::CreateViewParams params;
                                         params.size = m_size;
                                         params.universe = getEngine()->getCurrentUniverse(); // TODO
                                         params.target = nullptr;
                                         params.type = m_viewType;

                m_view = new ForwardView(params);
                Renderer::get()->AddView(m_view);
                draw = false;
            }

            if (!m_texture || m_texture->GetWidth() != m_size.x || m_texture->GetHeight() != m_size.y)
            {
                // As we're executing framegraph we can't fully release the texture right now because it may be still in use
                //VG_SAFE_RELEASE(m_texture);
                if (m_texture)
                    Device::get()->releaseAsync(m_texture);

                driver::TextureDesc targetDesc = driver::TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm_sRGB, TextureFlags::RenderTarget, m_size.x, m_size.y);
                string targetName = RenderContext::MakeUniqueName("Dest", m_view->GetViewID());
                m_texture = (driver::ITexture *)Device::get()->createTexture(targetDesc, targetName);
                m_view->SetSize(m_size);
                m_view->SetRenderTarget(m_texture);

                // Tell the Renderer we resized stuff to clean up no more used buffers
                Renderer::get()->SetResized();

                draw = false;
            }
            
            if (draw)
            {
                auto imGuiAdapter = Renderer::get()->getImGuiAdapter();
                ImTextureID texID = imGuiAdapter->getImguiTextureID((Texture *)m_texture);
                ImGui::Image(texID, ImVec2((float)m_size.x, (float)m_size.y));

                if (ImGui::IsWindowFocused())
                    m_view->SetActive(true);
                else
                    m_view->SetActive(false);

                imGuiAdapter->releaseImguiTextureID(texID);
            }

            // Draw Border
            //ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(0, 255, 0, 255));
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::End();


    }
}