#include "ImguiView.h"
#include "graphics/renderer/IView.h"
#include "graphics/driver/ITexture.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    ImGuiView::ImGuiView(const string & _name, Flags _flags) :
        ImguiEditor(_name, _flags)
    {
        
    }

    //--------------------------------------------------------------------------------------
    ImGuiView::~ImGuiView()
    {
        VG_SAFE_RELEASE(m_texture);
        Renderer::get()->ReleaseView(m_view);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::update(double _dt)
    {
 
    }

    //--------------------------------------------------------------------------------------
    void ImGuiView::display()
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(256, 256), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
        if (ImGui::Begin("View", &m_isVisible))
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
                CreateViewParams params;
                params.size = m_size;
                params.universe = getEngine()->getCurrentUniverse();
                params.target = nullptr;

                m_view = Renderer::get()->CreateView(params);
                draw = false;
            }

            if (!m_texture || m_texture->GetWidth() != m_size.x || m_texture->GetHeight() != m_size.y)
            {
                // As we're executing framegraph we can't fully release the texture right now because it may be still in use
                //VG_SAFE_RELEASE(m_texture);
                if (m_texture)
                    Device::get()->releaseAsync(m_texture);

                driver::TextureDesc targetDesc = driver::TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm_sRGB, TextureFlags::RenderTarget, m_size.x, m_size.y);
                m_texture = (driver::ITexture *)Device::get()->createTexture(targetDesc, "Target");
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
                imGuiAdapter->releaseImguiTextureID(texID);
            }

            // Draw Border
            ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(0, 255, 0, 255));
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::End();
    }
}