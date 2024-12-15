#include "UIManager.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    UIManager::UIManager()
    {

    }

    //--------------------------------------------------------------------------------------
    UIManager::~UIManager()
    {

    }

    //--------------------------------------------------------------------------------------
    void UIManager::AddCanvas(const UICanvas * _canvas, const UIItem & _desc, core::IWorld * _world)
    {
        addUIElement(UIElement(_canvas, _desc), _world);
    }

    //--------------------------------------------------------------------------------------
    void UIManager::AddText(const UICanvas * _canvas, const UIItem & _desc, const core::string & _text, Font _font, FontStyle _style, core::IWorld * _world)
    {
        addUIElement(UIElement(_canvas, _desc, _text, _font, _style), _world);
    }

    //--------------------------------------------------------------------------------------
    void UIManager::AddImage(const UICanvas * _canvas, const UIItem & _desc, const gfx::ITexture * _texture, core::IWorld * _world)
    {
        addUIElement(UIElement(_canvas, _desc, _texture), _world);
    }

    //--------------------------------------------------------------------------------------
    void UIManager::addUIElement(const UIElement & _elem, core::IWorld * _world)
    {
        auto * renderer = Renderer::get();
        auto * canvas = _elem.m_canvas;

        // Default
        gfx::ViewportTarget viewportTarget = gfx::ViewportTarget::Game;
        gfx::ViewportIndex viewportIndex = 0;

        if (canvas)
        {
            viewportTarget = canvas->m_viewportTarget;
            viewportIndex = canvas->m_viewportIndex;
        }

        if (auto * viewport = renderer->GetViewport(gfx::ViewportID(viewportTarget, viewportIndex)))
        {
            if (canvas && canvas->m_useViewMask)
            {
                const auto & views = viewport->GetViewIDs();
                for (const auto & pairs : views)
                {
                    const auto index = pairs.first;

                    if (asBool(((ViewMask)(1 << index)) & canvas->m_viewMask))
                    {
                        if (auto * view = renderer->GetView(pairs.second))
                        {
                            if (view->IsRender() && view->GetWorld() == _world)
                                view->GetUIRenderer()->Add(_elem);
                        }
                    }
                }
            }
            else
            {
                viewport->GetUIRenderer()->Add(_elem);
            }
        
            // Prefab preview mode
            if (IView * view = renderer->GetView(gfx::ViewTarget::Editor, _world))
            {
                if (view->IsRender() && view->GetWorld() == _world && asBool(ViewFlags::Prefab & view->GetFlags()))
                    view->GetUIRenderer()->Add(_elem);
            }
        }
    }
}