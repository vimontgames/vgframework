namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::string & ImGuiWindow::getIcon() const
    {
        return m_icon;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::string ImGuiWindow::getIconizedName() const
    {
        return getIcon().empty()? GetName() : getIcon() + " " + GetName();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::string & ImGuiWindow::getPath() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ImGuiWindow::Flags ImGuiWindow::getFlags() const
    {
        return m_flags;
    }
}