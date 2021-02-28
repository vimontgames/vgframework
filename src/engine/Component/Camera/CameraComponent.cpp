#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "graphics/renderer/View/View.h"
#include "core/Entity/Entity.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    CameraComponent::CameraComponent() :
        core::Component("CameraComponent")
    {

    }

    //--------------------------------------------------------------------------------------
    CameraComponent::~CameraComponent()
    {
        VG_SAFE_RELEASE(m_view);
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::update(const Entity * _entity, double _dt)
    {
        const float4x4 & matrix = _entity->getMatrix();
        m_view->setViewInvMatrix(inverse(matrix));
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::setView(graphics::renderer::IView * _view)
    {
        if (_view != m_view)
        {
            VG_SAFE_INCREASE_REFCOUNT(_view);
            VG_SAFE_RELEASE(m_view);

            m_view = _view;
        }
    }
}