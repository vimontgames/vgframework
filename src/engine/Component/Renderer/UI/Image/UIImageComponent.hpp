#include "UIImageComponent.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(UIImageComponent, "UIImage", "Renderer", "UI Image Element", editor::style::icon::Image, 2);

    //--------------------------------------------------------------------------------------
    bool UIImageComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResource(UIImageComponent, m_image, "Image");

        return true;
    }

    //--------------------------------------------------------------------------------------
    UIImageComponent::UIImageComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        m_image.SetParent(this);
        m_image.RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    UIImageComponent::~UIImageComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::OnLoad()
    {
        super::OnLoad();
        m_image.RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::OnResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_image)
        {
            VG_PROFILE_CPU("UIImageComponent");

            auto * tex = VG_SAFE_STATIC_CAST(gfx::ITexture, _resource->GetObject());
            if (tex != m_texture)
            {
                m_texture = tex;
                VG_SAFE_INCREASE_REFCOUNT(m_texture);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::OnResourceUnloaded(IResource * _resource)
    {
        if (_resource == &m_image)
            VG_SAFE_RELEASE_ASYNC(m_texture);
    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::Update(const Context & _context)
    {
        getUIManager()->AddImage(getCanvas(), getUIItem(), m_texture, _context.m_world);
    }
}