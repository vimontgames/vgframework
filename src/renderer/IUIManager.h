#pragma once
#include "core/Object/Object.h"

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
    enum class Font : vg::core::u8;
    enum class FontStyle : vg::core::u8;

    class IUIManager : public core::Object
    {
    public:
        IUIManager() {}
        virtual ~IUIManager() {}

        virtual void    AddCanvas   (const UICanvas * _canvas, const UIItem & _desc, core::IWorld * _world) = 0;
        virtual void    AddText     (const UICanvas * _canvas, const UIItem & _desc, const core::string & _text, Font _font, FontStyle _style, core::IWorld * _world) = 0;
        virtual void    AddImage    (const UICanvas * _canvas, const UIItem & _desc, const gfx::ITexture * _texture, core::IWorld * _world) = 0;
    };
}