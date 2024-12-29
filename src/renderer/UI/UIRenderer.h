#pragma once

#include "renderer/IUIRenderer.h"
#include "UIElement.h"

namespace vg::renderer
{
    class UIRenderer : public IUIRenderer
    {
    public:
        UIRenderer(IViewport * _viewport, IView * _view);
        ~UIRenderer();

        void            Add                 (const UIElement & _desc) final override;

        void            RenderFullscreen    (RenderUIType _renderUIType) final override;
        void            RenderWindowed      (RenderUIType _renderUIType) final override;

        void            Clear               () final override;

    protected:
        void            render              (RenderUIType _renderUIType);
        core::uint2     getSize             () const;
        core::float2    getScale            () const;
        core::float2    getOffset           () const;
        
    private:
        IViewport *                 m_viewport = nullptr;
        IView *                     m_view = nullptr;

        struct UIElementInfo
        {
            UIElement element;
            core::uint index;
        };

        core::vector<UIElementInfo> m_uiElements;
    };
}