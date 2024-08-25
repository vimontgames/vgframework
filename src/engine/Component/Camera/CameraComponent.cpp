#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/string/string.h"
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

        registerPropertyEnumEx(CameraComponent, gfx::ViewportTarget, m_target, "Target", IProperty::Flags::ReadOnly);
        setPropertyDescription(CameraComponent, m_target, "Camera target");

        registerPropertyEx(CameraComponent, m_viewportIndex, "Viewport", IProperty::Flags::ReadOnly);
        setPropertyRange(CameraComponent, m_viewportIndex, float2(0, 15));
        setPropertyDescription(CameraComponent, m_viewportIndex, "Viewport index");

        registerProperty(CameraComponent, m_viewIndex, "View");
        setPropertyRange(CameraComponent, m_viewIndex, float2(0, 15));
        setPropertyDescription(CameraComponent, m_viewIndex, "View index in current viewport");

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
        m_target(gfx::ViewportTarget::Game),
        m_viewportIndex((gfx::ViewportIndex)0),
        m_viewIndex((gfx::ViewIndex)0),
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
    gfx::ViewID CameraComponent::getViewID() const
    {
        if (auto * viewport = Engine::get()->GetRenderer()->GetViewport(gfx::ViewportID(m_target, m_viewportIndex)))
        {
            const auto & views = viewport->GetViewIDs();
            auto it = views.find(m_viewIndex);
            if (it != views.end())
                return it->second;
        }

        VG_WARNING("[Camera] Camera \"%s\" could not get ViewID for %s target at index %u", getName().c_str(), asString(m_target).c_str(), m_viewportIndex);
        return {};
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::Update(const Context & _context)
    {
        if (any(m_previousViewportScale != m_viewportScale))
        {
            Engine::get()->GetRenderer()->SetResized();
            m_previousViewportScale = m_viewportScale;
        }

        // Create or get the view
        if (auto * viewport = Engine::get()->GetRenderer()->GetViewport(gfx::ViewportID(m_target, m_viewportIndex)))
        {
            const auto & views = viewport->GetViewIDs();
            auto it = views.find(m_viewIndex);
            if (it == views.end())
            {
                gfx::CreateViewParams viewParams;
                                      viewParams.size = viewport->GetRenderTargetSize();
                                      viewParams.world = GetGameObject()->GetWorld();
                                      viewParams.target = (gfx::ViewTarget)m_target;  // TODO: hazardous cast between ViewTarget and ViewportTarget? Create single enum?
                                      viewParams.viewport = viewport;

                string viewName = fmt::sprintf("%s View #%u", asString(viewParams.target).c_str(), m_viewIndex);

                // Create a view
                auto renderer = Engine::get()->GetRenderer();
                if (auto * view = renderer->CreateView(viewParams, viewName))
                {
                    view->SetRenderTarget(viewport->GetRenderTarget());
                    view->SetRenderTargetSize(viewport->GetRenderTargetSize());
                    renderer->AddView(view);
                    viewport->AddView(m_viewIndex, view->GetViewID());
                    VG_SAFE_RELEASE(view);
                }
            }
        }

        auto * view = Engine::get()->GetRenderer()->GetView(getViewID());
        if (nullptr != view)
        {
            view->SetRender(true);
            const float4x4 & matrix = getGameObject()->GetGlobalMatrix();
            view->SetupPerspectiveCamera(matrix, float2(m_near, m_far), m_fovY, m_viewportOffset, m_viewportScale);
        }      
    }
}