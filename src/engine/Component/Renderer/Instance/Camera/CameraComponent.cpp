#include "engine/Precomp.h"
#include "CameraComponent.h"
#include "core/GameObject/GameObject.h"
#include "core/Math/Math.h"
#include "core/string/string.h"
#include "renderer/IRenderer.h"
#include "renderer/ICameraLens.h"
#include "renderer/IPicking.h"
#include "renderer/ICameraInstance.h"
#include "engine/Engine.h"
#include "editor/Editor_Consts.h"

#if !VG_ENABLE_INLINE
#include "CameraComponent.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(CameraComponent, "Camera", "Renderer", "Setup camera settings for a View", editor::style::icon::Camera, 1);

    //--------------------------------------------------------------------------------------
    bool CameraComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyGroupBegin(CameraComponent, "Viewport");
        {
            registerPropertyEnumEx(CameraComponent, gfx::ViewportTarget, m_target, "Target", PropertyFlags::ReadOnly | PropertyFlags::Hidden);
            setPropertyDescription(CameraComponent, m_target, "Camera target");

            registerPropertyEx(CameraComponent, m_viewportIndex, "Viewport", PropertyFlags::ReadOnly | PropertyFlags::Hidden);
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
        }
        registerPropertyGroupEnd(CameraComponent);

        registerPropertyObjectPtrEx(CameraComponent, m_cameraSettings, "Camera Settings", PropertyFlags::Flatten);

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
        m_cameraSettings = (ICameraSettings*)Kernel::getFactory()->CreateObject("CameraSettings");
        m_cameraSettings->RegisterUID();
        m_cameraSettings->SetParent(this);

        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        m_pickingID = picking->CreatePickingID(this);

        m_cameraInstance = (ICameraInstance *)CreateFactoryObject(CameraInstance, _name, this);
        auto * go = getGameObject();
        m_cameraInstance->SetName(go->GetName().c_str());
        m_cameraInstance->SetPickingID(m_pickingID);

        //if (!m_registered)
        {
            go->addGraphicInstance(m_cameraInstance);   // Make ICameraComponent a child of GraphicInstanceComponent?
            //m_registered = true;
        }
    }

    //--------------------------------------------------------------------------------------
    CameraComponent::~CameraComponent()
    {
        VG_SAFE_RELEASE(m_cameraSettings);

        auto * renderer = Engine::get()->GetRenderer();
        auto * picking = renderer->GetPicking();
        picking->ReleasePickingID(m_pickingID);

        auto * go = getGameObject();
        go->removeGraphicInstance(m_cameraInstance);
        Engine::get()->GetRenderer()->ReleaseAsync(m_cameraInstance);
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

        if (strcmp(_prop.GetName(), "m_viewportScale"))
            Engine::get()->GetRenderer()->SetResized();
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::EnableComponentFlags(core::ComponentFlags _flags, bool _enabled)
    {
        super::EnableComponentFlags(_flags, _enabled);

        if (m_cameraInstance)
            m_cameraInstance->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::updateLensConstraints()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::Update(const Context & _context)
    {
        auto renderer = Engine::get()->GetRenderer();

        // Create or get the view
        IViewport * viewport = Engine::get()->GetRenderer()->GetViewport(gfx::ViewportID(m_target, m_viewportIndex));
        if (viewport)
        {
            const auto & views = viewport->GetViewIDs();
            auto it = views.find(m_viewIndex);
            if (it == views.end())
            {
                CreateViewParams viewParams;
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
            if (renderer->IsGameMode())
            {
                view->SetRenderTarget(nullptr);
                view->SetRenderTargetSize(viewport->GetRenderTargetSize());
                view->SetVisible(true);
            }

            VG_ASSERT(nullptr != m_cameraInstance);
            m_cameraInstance->setGlobalMatrix(_context.m_gameObject->GetGlobalMatrix());
            m_cameraInstance->setColor(_context.m_gameObject->GetColor());

            view->SetRender(true);
            const auto & go = getGameObject();
            const float4x4 & matrix = go->GetGlobalMatrix();
            view->SetupPhysicalCamera(matrix, m_cameraSettings, m_viewportOffset, m_viewportScale, go, m_pickingID);
        }      
    }

    //--------------------------------------------------------------------------------------
    void CameraComponent::SetViewportOffsetAndScale(core::float2 _offset, const core::float2 & _scale)
    {
        m_viewportOffset = _offset;

        if (any(m_viewportScale != _scale))
        {
            m_viewportScale = _scale;
            Engine::get()->GetRenderer()->SetResized();
        }
    }

    //--------------------------------------------------------------------------------------
    renderer::ICameraSettings * CameraComponent::GetCameraSettings() const
    {
        return m_cameraSettings;
    }
}