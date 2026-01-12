#pragma once
#include "engine/IUIImageComponent.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class UIImageComponent : public IUIImageComponent
    {
    public:
        VG_CLASS_DECL(UIImageComponent, IUIImageComponent);

        UIImageComponent(const core::string & _name, core::IObject * _parent);
        ~UIImageComponent();

        void    OnLoad              () override;

        void    OnResourceLoaded    (IResource * _resource) final override;
        void    OnResourceUnloaded  (IResource * _resource) final override;

        void    Update              (const Context & _context) final override;

    private:
        TextureResource m_image;
        gfx::ITexture * m_texture = nullptr;
    };
}