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
    VG_REGISTER_COMPONENT_CLASS(CameraComponent, "Camera", "Rendering", "Setup camera settings for a View", editor::style::icon::Camera, 1);

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(CameraComponent, gfx::ViewTarget, m_target, "Target");
        setPropertyDescription(CameraComponent, m_target, "Camera target");

        registerProperty(CameraComponent, m_index, "Index");
        setPropertyRange(CameraComponent, m_index, float2(0, 15));
        setPropertyDescription(CameraComponent, m_index, "Camera target index");

        registerProperty(CameraComponent, m_fovY, "FOV");
        setPropertyRange(CameraComponent, m_fovY, float2(PI / 8.0f, PI / 2.0f));
        setPropertyDescription(CameraComponent, m_fovY, "Horizontal Field-Of-View");

        registerProperty(CameraComponent, m_near, "Near");
        setPropertyRange(CameraComponent, m_near, float2(0.0f, 10.0f));
        setPropertyDescription(CameraComponent, m_near, "Camera near distance");

        registerProperty(CameraComponent, m_far, "Far");
        setPropertyRange(CameraComponent, m_far, float2(0.0f, 10000.0f));
        setPropertyDescription(CameraComponent, m_far, "Camera far distance");

        registerProperty(CameraComponent, m_viewportOffset, "Offset");
        setPropertyRange(CameraComponent, m_viewportOffset, float2(0.0f, 1.0f));
        setPropertyDescription(CameraComponent, m_viewportOffset, "Viewport offset");

        registerProperty(CameraComponent, m_viewportScale, "Scale");
        setPropertyRange(CameraComponent, m_viewportScale, float2(0.0f, 1.0f));
        setPropertyDescription(CameraComponent, m_viewportScale, "Viewport scale");

        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraComponent::CameraComponent(const core::string & _name, core::IObject * _parent) :
        core::Component(_name, _parent),
        m_viewportOffset(float2(0, 0)),
        m_viewportScale(float2(1, 1)),
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
    void CameraComponent::Update(const Context & _context)
    {
        if (any(m_previousViewportScale != m_viewportScale))
        {
            Engine::get()->GetRenderer()->SetResized();
            m_previousViewportScale = m_viewportScale;
        }

        auto * view = Engine::get()->GetRenderer()->GetView(gfx::ViewID(m_target, m_index));
        if (nullptr != view)
        {
            const float4x4 & matrix = getGameObject()->GetGlobalMatrix();
            view->SetupPerspectiveCamera(matrix, float2(m_near, m_far), m_fovY, m_viewportOffset, m_viewportScale);
        }
    }
}