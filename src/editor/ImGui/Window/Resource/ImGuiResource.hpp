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

            if (ImGui::TextButton("Press 'F7' to", "Reload Resources", rm != nullptr && rm->GetResourceCount() > 0 && !rm->HasResourceLoading(), "Reimport all modified resources"))
                rm->UpdateResources();
            
            if (nullptr != rm)
                displayObject(rm); 
        }  
        ImGui::End();  
    }
}