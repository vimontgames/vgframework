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
    void UIManager::AddCanvas(const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, core::IWorld * _world)
    {
        addUIElement(UIElement(_canvas, _desc), _world);
    }

    //--------------------------------------------------------------------------------------
    void UIManager::AddText(const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const core::string & _text, Font _font, FontStyle _style, core::IWorld * _world)
    {
        addUIElement(UIElement(_canvas, _desc, _text, _font, _style), _world);
    }

    //--------------------------------------------------------------------------------------
    void UIManager::AddImage(const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const gfx::ITexture * _texture, core::IWorld * _world)
    {
        addUIElement(UIElement(_canvas, _desc, _texture), _world);
    }

    //--------------------------------------------------------------------------------------
    void UIManager::addUIElement(const UIElement & _elem, core::IWorld * _world)
    {
        auto * renderer = Renderer::get();
        auto * canvas = _elem.m_canvas;
        VG_ASSERT(canvas);
        
        if (nullptr != canvas)
        {
            if (auto * viewport = renderer->GetViewport(gfx::ViewportID(canvas->m_viewportTarget, canvas->m_viewportIndex)))
            {
                if (canvas->m_useViewMask)
                {
                    const auto & views = viewport->GetViewIDs();
                    for (const auto & pairs : views)
                    {
                        const auto index = pairs.first;

                        if (asBool(((gfx::ViewMask)(1 << index)) & canvas->m_viewMask))
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
                if (gfx::IView * view = renderer->GetView(gfx::ViewTarget::Editor, _world))
                {
                    if (view->IsRender() && view->GetWorld() == _world && asBool(gfx::ViewFlags::Prefab & view->GetFlags()))
                        view->GetUIRenderer()->Add(_elem);
                }
            }
        }
    }
}