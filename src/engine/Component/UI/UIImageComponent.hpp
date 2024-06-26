#include "UIImageComponent.h"
#include "editor/Editor_Consts.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"
#include "gfx/ITexture.h"
#include "core/IGameObject.h"
#include "gfx/IViewGUI.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(UIImageComponent, "UIImage", "UI", "UI Image Element", editor::style::icon::Image, 2);

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
        m_image.setParent(this);
    }

    //--------------------------------------------------------------------------------------
    UIImageComponent::~UIImageComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::onResourceLoaded(IResource * _resource)
    {
        if (_resource == &m_image)
        {
            auto * tex = VG_SAFE_STATIC_CAST(gfx::ITexture, _resource->getObject());
            if (tex != m_texture)
            {
                m_texture = tex;
                VG_SAFE_INCREASE_REFCOUNT(m_texture);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::onResourceUnloaded(IResource * _resource)
    {
        if (_resource == &m_image)
            VG_SAFE_RELEASE_ASYNC(m_texture);
    }

    //--------------------------------------------------------------------------------------
    void UIImageComponent::Update(float _dt)
    {
        if (IWorld * world = GetGameObject()->GetWorld())
        {
            auto * renderer = Engine::get()->GetRenderer();
            if (gfx::IView * view = renderer->GetView(gfx::ViewTarget::Game, world))
            {
                if (auto * gui = view->GetViewGUI())
                    gui->AddImage(m_pickingID, getMatrix(), getColor(), m_horizontal, m_vertical, m_texture);
            }
        }
    }
}