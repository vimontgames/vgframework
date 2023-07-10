#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "graphics/renderer/View/View.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "engine/Engine.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(CameraComponent);

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(CameraComponent, "Camera Component", IClassDesc::Flags::Component))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyHelper(CameraComponent, m_fovY, "Horizontal FOV", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(CameraComponent, m_fovY, float2(pi / 8.0f, pi / 2.0f));

        _desc.registerPropertyHelper(CameraComponent, m_near, "Near", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(CameraComponent, m_near, float2(0.0f, 8.0f));

        _desc.registerPropertyHelper(CameraComponent, m_far, "Far", IProperty::Flags::None);
        _desc.setPropertyRangeHelper(CameraComponent, m_far, float2(0.0f, 8192.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraComponent::CameraComponent(const core::string & _name, core::IObject * _parent) :
        core::Component(_name, _parent),
        m_fovY(pi / 4.0f),
        m_near(0.1f),
        m_far(1024.0f)
    {

    }

    //--------------------------------------------------------------------------------------
    CameraComponent::~CameraComponent()
    {
        VG_SAFE_RELEASE(m_view);
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::Update(double _dt)
    {
        if (m_view == nullptr)
        {
            m_view = Engine::get()->getEditorView(); // Quick hack, TODO: multiview management
            VG_SAFE_INCREASE_REFCOUNT(m_view);
        }

        const float4x4 & matrix = getGameObject()->getWorldMatrix();
        m_view->SetupCamera(inverse(matrix), float2(m_near, m_far), m_fovY);
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::setView(graphics::renderer::IView * _view, core::IGameObject * _sector)
    {
        if (_view != m_view)
        {
            VG_SAFE_RELEASE(m_view);
            m_view = _view;
            VG_SAFE_INCREASE_REFCOUNT(m_view);
        }

        if (nullptr != m_view)
            m_view->SetCameraSector(_sector);
    }
}