#pragma once
#include "core/Object/Object.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::gfx
{
    class ITexture;
    struct UICanvas;
    struct UIItem;
}

namespace vg::renderer
{
    enum class Font : vg::core::u8;
    enum class FontStyle : vg::core::u8;

    class IUIManager : public core::Object
    {
    public:
        IUIManager() {}
        virtual ~IUIManager() {}

        virtual void    AddCanvas   (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, core::IWorld * _world) = 0;
        virtual void    AddText     (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const core::string & _text, Font _font, FontStyle _style, core::IWorld * _world) = 0;
        virtual void    AddImage    (const gfx::UICanvas * _canvas, const gfx::UIItem & _desc, const gfx::ITexture * _texture, core::IWorld * _world) = 0;
    };
}