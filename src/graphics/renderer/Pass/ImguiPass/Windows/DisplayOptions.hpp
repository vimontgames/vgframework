namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPass::displayOptionsWindow()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
        {
            if (ImGui::Begin("Display Options", &m_isDisplayOptionsWindowsVisible))
                displayObject(displayOptions);

            ImGui::End();
        }
    }
}