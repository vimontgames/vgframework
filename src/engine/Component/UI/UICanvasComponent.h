#pragma once
#include "engine/IUICanvasComponent.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class UICanvasComponent final : public IUICanvasComponent
    {
    public:
        VG_CLASS_DECL(UICanvasComponent, IUICanvasComponent);

        UICanvasComponent(const core::string & _name, core::IObject * _parent);
        ~UICanvasComponent();

        void                            SetViewIndex    (core::uint _viewIndex) final override;
        void                            Update          (const Context & _context) final override;

        VG_INLINE const gfx::UICanvas & getGfxCanvas    () const { return m_canvas; }

    private:
        gfx::UICanvas m_canvas;
    };
}