#include "ImGuiResource.h"
#include "engine/IResourceManager.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    void ImGuiResource::DrawGUI()
    {
        if (ImGui::IconBegin(style::icon::Resource, "Resources", &m_isVisible))
        {
            engine::IResourceManager * rm = Editor::get()->getEngine()->GetResourceManager();
            if (nullptr != rm)
                displayObject(rm); 
        }  
        ImGui::End();  
    }
}