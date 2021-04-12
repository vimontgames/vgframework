#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "graphics/renderer/View/View.h"
#include "core/Entity/Entity.h"
#include "core/Object/AutoRegisterClass.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_COMPONENT(CameraComponent);

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerClass(IObjectFactory & _factory)
    {
        if (core::IObjectDescriptor * desc = _factory.registerClassHelper(CameraComponent, "Camera Component"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraComponent::CameraComponent(const core::string & _name, core::IObject * _parent) :
        core::Component(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    CameraComponent::~CameraComponent()
    {
        VG_SAFE_RELEASE(m_view);
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::update(double _dt)
    {
        const float4x4 & matrix = getEntity()->GetWorldMatrix();
        m_view->SetViewInvMatrix(inverse(matrix));
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::setView(graphics::renderer::IView * _view, core::ISector * _sector)
    {
        if (_view != m_view)
        {
            //VG_SAFE_INCREASE_REFCOUNT(_view);
            //VG_SAFE_RELEASE(m_view);

            m_view = _view;
        }

        if (nullptr != m_view)
            m_view->SetCameraSector(_sector);
    }
}