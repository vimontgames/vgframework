#include "engine/IResourceManager.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPass::displayResourcesWindow()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
        {
            if (ImGui::Begin("Resources", &m_isWindowVisible[asInteger(UIWindow::Resources)]))
            {
                engine::IResourceManager * rm = getEngine()->GetResourceManager();
                if (nullptr != rm)
                    displayObject(rm);
            }

            ImGui::End();
        }
    }
}