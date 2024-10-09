#pragma once

#include "gfx/IUIRenderer.h"
#include "UIElement.h"

namespace vg::renderer
{
    class UIRenderer : public gfx::IUIRenderer
    {
    public:
        UIRenderer(gfx::IViewport * _viewport, gfx::IView * _view);
        ~UIRenderer();

        void            Add                 (const UIElement & _desc) final override;

        void            RenderFullscreen    () final override;
        void            RenderWindowed      () final override;

        void            Clear               () final override;

    protected:
        void            render              ();
        core::uint2     getSize             () const;
        core::float2    getScale            () const;
        core::float2    getOffset           () const;
        
    private:
        gfx::IViewport *            m_viewport = nullptr;
        gfx::IView *                m_view = nullptr;
        core::vector<UIElement>     m_uiElements;
    };
}