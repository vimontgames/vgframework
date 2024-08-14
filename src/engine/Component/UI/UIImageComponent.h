#pragma once
#include "engine/Component/UI/UIComponent.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class UIImageComponent : public UIComponent
    {
    public:
        VG_CLASS_DECL(UIImageComponent, UIComponent);
        UIImageComponent(const core::string & _name, core::IObject * _parent);
        ~UIImageComponent();

        void    onResourceLoaded    (IResource * _resource) final override;
        void    onResourceUnloaded  (IResource * _resource) final override;

        void    Update              (const Context & _context) final override;

    private:
        TextureResource m_image;
        gfx::ITexture * m_texture = nullptr;
    };
}