#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "gfx/IView.h"
#include "renderer/IRenderer.h"
#include "engine/Engine.h"
#include "editor/Editor_Consts.h"

#if !VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(CameraComponent, "Camera", "Rendering", "Setup camera settings for a View", editor::style::icon::Camera);

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(CameraComponent, gfx::ViewTarget, m_target, "Target");
        registerProperty(CameraComponent, m_index, "Index");
        setPropertyRange(CameraComponent, m_index, float2(0, 15));

        registerProperty(CameraComponent, m_fovY, "Horizontal FOV");
        setPropertyRange(CameraComponent, m_fovY, float2(PI / 8.0f, PI / 2.0f));

        registerProperty(CameraComponent, m_near, "Near");
        setPropertyRange(CameraComponent, m_near, float2(0.0f, 8.0f));

        registerProperty(CameraComponent, m_far, "Far");
        setPropertyRange(CameraComponent, m_far, float2(0.0f, 8192.0f));

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
    void CameraComponent::Update(float _dt)
    {
        auto * view = Engine::get()->GetRenderer()->GetView(gfx::ViewID(m_target, m_index));
        if (nullptr != view)
        {
            const float4x4 & matrix = getGameObject()->GetGlobalMatrix();
            view->SetupPerspectiveCamera(matrix, float2(m_near, m_far), m_fovY);
        }
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {

    }
}