#pragma once

namespace vg::graphics::renderer
{
    class ImguiMenu
    {
    public:
        virtual void    Display(core::IObject * _object) = 0;

    protected:
        core::uint      m_selected  = 0;
        const char *    m_popup;
    };
}