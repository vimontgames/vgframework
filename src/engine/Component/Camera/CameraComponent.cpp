#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/string/string.h"
#include "gfx/IView.h"
#include "renderer/IRenderer.h"
#include "renderer/ICameraLens.h"
#include "engine/Engine.h"
#include "editor/Editor_Consts.h"

#if !VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(CameraComponent, "Camera", "Renderer", "Setup camera settings for a View", editor::style::icon::Camera, 1);

    //--------------------------------------------------------------------------------------
    float2 getFocalLengthRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const auto * camera = ((CameraComponent *)_object);
        const renderer::ICameraLens * lens = camera->getCameraLens();
        return lens->GetFocalLengthRange();
    }

    //--------------------------------------------------------------------------------------
    core::float2 getFocusDistanceRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const auto * camera = ((CameraComponent *)_object);
        const renderer::ICameraLens * lens = camera->getCameraLens();
        
        return float2(lens->GetMinFocusDistance(camera->getFocalLength()), 1000.0f);
    }

    //--------------------------------------------------------------------------------------
    core::float2 getApertureRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const auto * camera = ((CameraComponent *)_object);
        const renderer::ICameraLens * lens = camera->getCameraLens();
        const float focalLength = camera->getFocalLength();
        return float2(lens->GetMaxAperture(focalLength), lens->GetMinAperture(focalLength));
    }

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(CameraComponent, gfx::ViewportTarget, m_target, "Target", PropertyFlags::ReadOnly | PropertyFlags::NotVisible);
        setPropertyDescription(CameraComponent, m_target, "Camera target");

        registerPropertyEx(CameraComponent, m_viewportIndex, "Viewport", PropertyFlags::ReadOnly | PropertyFlags::NotVisible);
        setPropertyRange(CameraComponent, m_viewportIndex, float2(0, 15));
        setPropertyDescription(CameraComponent, m_viewportIndex, "Viewport index");

        registerProperty(CameraComponent, m_viewIndex, "Index");
        setPropertyRange(CameraComponent, m_viewIndex, float2(0, 15));
        setPropertyDescription(CameraComponent, m_viewIndex, "View index in current viewport");

        registerProperty(CameraComponent, m_viewportOffset, "Offset");
        setPropertyRange(CameraComponent, m_viewportOffset, float2(0.0f, 1.0f));
        setPropertyDescription(CameraComponent, m_viewportOffset, "Viewport offset");

        registerProperty(CameraComponent, m_viewportScale, "Scale");
        setPropertyRange(CameraComponent, m_viewportScale, float2(0.0f, 1.0f));
        setPropertyDescription(CameraComponent, m_viewportScale, "Viewport scale");

        registerPropertyResource(CameraComponent, m_lensRes, "Lens");

        registerProperty(CameraComponent, m_physicalCameraSettings.m_focalLength, "Focal length");
        setPropertyDescription(CameraComponent, m_physicalCameraSettings.m_focalLength, "Focal length in millimeters");
        setPropertyRangeCallback(CameraComponent, m_physicalCameraSettings.m_focalLength, getFocalLengthRange); // Range depends on the lens used

        registerProperty(CameraComponent, m_focusDistance, "Focus distance");
        setPropertyDescription(CameraComponent, m_focusDistance, "The distance in meters between the lens and the subject that is in sharp focus");
        setPropertyRangeCallback(CameraComponent, m_focusDistance, getFocusDistanceRange); // Range depends on the lens used and focal length

        registerProperty(CameraComponent, m_aperture, "f/Aperture");
        setPropertyDescription(CameraComponent, m_aperture, "A higher f-stop (e.g., f/16) means a smaller aperture that lets in less light, creating a deeper depth of field");
        setPropertyRangeCallback(CameraComponent, m_aperture, getApertureRange); // Range depends on the lens used and focal length

        registerPropertyEnum(CameraComponent, gfx::GateFitMode, m_physicalCameraSettings.m_gateFitMode, "Gate Fit");
        setPropertyDescription(CameraComponent, m_physicalCameraSettings.m_gateFitMode, "Determines how the camera aligns its field of view to fit the sensor");

        registerProperty(CameraComponent, m_physicalCameraSettings.m_near, "Near");
        setPropertyRange(CameraComponent, m_physicalCameraSettings.m_near, float2(0.0f, 10.0f));
        setPropertyDescription(CameraComponent, m_physicalCameraSettings.m_near, "Camera near distance");

        registerProperty(CameraComponent, m_physicalCameraSettings.m_far, "Far");
        setPropertyRange(CameraComponent, m_physicalCameraSettings.m_far, float2(0.0f, 10000.0f));
        setPropertyDescription(CameraComponent, m_physicalCameraSettings.m_far, "Camera far distance");
  
        return true;
    }

    //--------------------------------------------------------------------------------------
    CameraComponent::CameraComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_target(gfx::ViewportTarget::Game),
        m_viewportIndex((gfx::ViewportIndex)0),
        m_viewIndex((gfx::ViewIndex)0),
        m_viewportOffset(float2(0, 0)),
        m_viewportScale(float2(1, 1))
    {
        m_lensRes.SetParent(this);
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

        VG_WARNING("[Camera] Camera \"%s\" could not get ViewID for %s target at index %u", GetName().c_str(), asString(m_target).c_str(), m_viewportIndex);
        return {};
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::OnLoad()
    {
        updateLensConstraints();
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::OnPropertyChanged(vg::core::IObject * _object, const vg::core::IProperty & _prop, bool _notifyParent)
    {
        updateLensConstraints();
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::updateLensConstraints()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::Update(const Context & _context)
    {
        auto renderer = Engine::get()->GetRenderer();

        if (any(m_previousViewportScale != m_viewportScale))
        {
            Engine::get()->GetRenderer()->SetResized();
            m_previousViewportScale = m_viewportScale;
        }

        // Create or get the view
        gfx::IViewport * viewport = Engine::get()->GetRenderer()->GetViewport(gfx::ViewportID(m_target, m_viewportIndex));
        if (viewport)
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

        auto * view = renderer->GetView(getViewID());
        if (nullptr != view)
        {
            if (renderer->IsFullscreen())
            {
                view->SetRenderTarget(nullptr);
                view->SetRenderTargetSize(viewport->GetRenderTargetSize());
                view->SetVisible(true);
            }

            view->SetRender(true);
            const float4x4 & matrix = getGameObject()->GetGlobalMatrix();
            view->SetupPhysicalCamera(matrix, m_physicalCameraSettings.m_focalLength, getCameraLens()->GetSensorSize(), m_physicalCameraSettings.m_gateFitMode, m_physicalCameraSettings.m_near, m_physicalCameraSettings.m_far, m_viewportOffset, m_viewportScale);
        }      
    }

    //--------------------------------------------------------------------------------------
    const renderer::ICameraLens * CameraComponent::getCameraLens() const
    {
        if (auto * camLens = VG_SAFE_STATIC_CAST(renderer::ICameraLens, m_lensRes.GetObject()))
            return camLens;
        else
            return Engine::get()->GetRenderer()->GetDefaultCameraLens();
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::SetViewportOffsetAndScale(core::float2 _offset, const core::float2 & _scale)
    {
        m_viewportOffset = _offset;
        m_viewportScale = _scale;
    }
}