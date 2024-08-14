#pragma once
#include "engine/Component/UI/UIComponent.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class UICanvasComponent : public UIComponent
    {
    public:
        VG_CLASS_DECL(UICanvasComponent, UIComponent);
        UICanvasComponent(const core::string & _name, core::IObject * _parent);
        ~UICanvasComponent();

        void    Update(const Context & _context) final override;

        VG_INLINE const gfx::UICanvas & getGfxCanvas() const { return m_canvas; }

    private:
        gfx::UICanvas m_canvas;
    };
}