#include "engine/Precomp.h"
#include "CameraComponent.h"

#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"

#include "graphics/driver/IView.h"
#include "graphics/renderer/IRenderer.h"

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

    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::Update(double _dt)
    {
        // TEMP: Update all views
        const auto & views = Engine::get()->GetRenderer()->GetViews();
        for (uint i = 0; i < views.count(); ++i)
        {
            auto * view = views[i];
            const float4x4 & matrix = getGameObject()->getWorldMatrix();
            view->SetupCamera(inverse(matrix), float2(m_near, m_far), m_fovY);
        }
    }
}