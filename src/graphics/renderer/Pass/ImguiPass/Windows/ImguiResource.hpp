#include "ImguiResource.h"
#include "imgui/imgui.h"
#include "engine/IResourceManager.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiResource::display()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
        {
            if (ImGui::Begin("Resources", &m_isVisible))
            {
                engine::IResourceManager * rm = getEngine()->GetResourceManager();
                if (nullptr != rm)
                    displayObject(rm);
            }

            ImGui::End();
        }
    }
}