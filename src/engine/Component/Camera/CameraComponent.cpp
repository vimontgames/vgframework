#include "engine/Precomp.h"
#include "CameraComponent.h"

#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"

#include "gfx/IView.h"
#include "graphics/renderer/IRenderer.h"

#include "engine/Engine.h"

#if !VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif

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

        _desc.registerPropertyEnum(CameraComponent, gfx::ViewType, m_ViewType, "View Type");
        _desc.registerPropertyHelper(CameraComponent, m_ViewIndex, "View Index");
        _desc.setPropertyRangeHelper(CameraComponent, m_ViewIndex, float2(0, 15));

        _desc.registerPropertyHelper(CameraComponent, m_fovY, "Horizontal FOV");
        _desc.setPropertyRangeHelper(CameraComponent, m_fovY, float2(PI / 8.0f, PI / 2.0f));

        _desc.registerPropertyHelper(CameraComponent, m_near, "Near");
        _desc.setPropertyRangeHelper(CameraComponent, m_near, float2(0.0f, 8.0f));

        _desc.registerPropertyHelper(CameraComponent, m_far, "Far");
        _desc.setPropertyRangeHelper(CameraComponent, m_far, float2(0.0f, 8192.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraComponent::CameraComponent(const core::string & _name, core::IObject * _parent) :
        core::Component(_name, _parent),
        m_fovY(PI / 4.0f),
        m_near(0.1f),
        m_far(1024.0f)
    {

    }

    //--------------------------------------------------------------------------------------
    CameraComponent::~CameraComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::Update(double _dt)
    {
        auto * view = Engine::get()->GetRenderer()->GetView(gfx::ViewID(m_ViewType, m_ViewIndex));
        if (nullptr != view)
        {
            const float4x4 & matrix = getGameObject()->getWorldMatrix();
            view->SetupCamera(inverse(matrix), float2(m_near, m_far), m_fovY);
        }
    }
}