#include "ImGuiResource.h"
#include "engine/IResourceManager.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiResource::ImGuiResource() :
        ImGuiWindow(style::icon::Resource, "", "Resources", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiResource::DrawGUI()
    {
        if (ImGui::IconBegin(style::icon::Resource, getName().c_str(), &m_isVisible))
        {
            engine::IResourceManager * rm = Editor::get()->getEngine()->GetResourceManager();
            if (nullptr != rm)
                displayObject(rm); 
        }  
        ImGui::End();  
    }
}