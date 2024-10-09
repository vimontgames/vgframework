#pragma once

#include "core/Singleton/Singleton.h"
#include "renderer/IUIManager.h"

namespace vg::renderer
{
    struct UIElement;

    class UIManager : public IUIManager, public core::Singleton<UIManager>
    {
    public:
        UIManager();
        ~UIManager();

        void    AddCanvas       (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, core::IWorld * _world) final override;
        void    AddText         (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const core::string & _text, Font _font, FontStyle _style, core::IWorld * _world) final override;
        void    AddImage        (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const gfx::ITexture * _texture, core::IWorld * _world) final override;

    private:
        void    addUIElement    (const UIElement & _elem, core::IWorld * _world);
    };
}