#pragma once
#include "core/Object/Object.h"
#include "renderer/ImGui_consts.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::gfx
{
    class ITexture;
}

namespace vg::renderer
{
    struct UICanvas;
    struct UIItem;

    class IUIManager : public core::Object
    {
    public:
        IUIManager() {}
        virtual ~IUIManager() {}

        virtual void    AddCanvas   (const UICanvas * _canvas, const UIItem & _desc, core::IWorld * _world) = 0;
        virtual void    AddText     (const UICanvas * _canvas, const UIItem & _desc, const core::string & _text, Font _font, FontStyle _fontStyle, FontSize _fontSize, core::IWorld * _world) = 0;
        virtual void    AddImage    (const UICanvas * _canvas, const UIItem & _desc, const gfx::ITexture * _texture, core::IWorld * _world) = 0;
    };
}