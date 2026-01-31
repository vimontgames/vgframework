#include "engine/Precomp.h"
#include "LightComponent.h"
#include "core/GameObject/GameObject.h"
#include "renderer/IRenderer.h"
#include "renderer/IPicking.h"
#include "renderer/ILightInstance.h"
#include "editor/Editor_Consts.h"
#include "engine/Engine.h"

#if !VG_ENABLE_INLINE
#include "LightComponent.inl"
#endif

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(LightComponent, "Light", "Renderer", "Light component to light up scene", editor::style::icon::Light, getPriority(ComponentGroup::Render, ComponentPriority::Late));

    //--------------------------------------------------------------------------------------
    bool LightComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(LightComponent, renderer::LightType, m_lightType, "Type");
        registerPropertyObjectPtrEx(LightComponent, m_lightDesc, "Light", PropertyFlags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    LightComponent::LightComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_lightType(renderer::LightType::Omni)
    {
        if (nullptr == m_lightDesc)
            createLightDesc();
        if (nullptr == m_light)
            createLight();

        UpdateFlagsFromGameObject();
    }

    //--------------------------------------------------------------------------------------
    LightComponent::~LightComponent()
    {
        VG_SAFE_RELEASE(m_lightDesc);

        if (m_registered)
        {
            auto * picking = Engine::get()->GetRenderer()->GetPicking();
            picking->ReleasePickingID(m_light->GetPickingID());
            m_light->ClearPickingID();

            getGameObject()->removeGraphicInstance(m_light);
            m_registered = false;
        }

        VG_SAFE_RELEASE_ASYNC(m_light);
    }

    //--------------------------------------------------------------------------------------
    bool LightComponent::createLightDesc()
    {
        IFactory * factory = Kernel::getFactory();
        VG_SAFE_RELEASE(m_lightDesc);

        switch (m_lightType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_lightType);
                break;

            case renderer::LightType::Directional:
                m_lightDesc = (renderer::ILightDesc *)factory->CreateObject("DirectionalLightDesc", "", this);
                break;

            case renderer::LightType::Omni:
                m_lightDesc = (renderer::ILightDesc *)factory->CreateObject("OmniLightDesc", "", this);
                break;
        }

        if (nullptr != m_lightDesc)
        {
            m_lightDesc->RegisterUID();
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool LightComponent::createLight()
    {
        IFactory * factory = Kernel::getFactory();

        if (m_registered)
            unregisterGraphicInstance();

        VG_SAFE_RELEASE_ASYNC(m_light);

        switch (m_lightType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_lightType);
                break;

            case renderer::LightType::Directional:
            case renderer::LightType::Omni:
                m_light = Engine::get()->GetRenderer()->CreateLightInstance(m_lightDesc);                
                break;
        }

        if (m_light)
        {
            m_light->SetName(GetGameObject()->GetName());

            auto * picking = Engine::get()->GetRenderer()->GetPicking();
            PickingID id = m_light->GetPickingID();
            if (!id)
            {
                id = picking->CreatePickingID(this);
                m_light->SetPickingID(id);
            }

            UpdateFlagsFromGameObject();

            if (GetGameObject()->IsEnabledInHierarchy())
                registerGraphicInstance();

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::OnLoad()
    {
        super::OnLoad();

        createLight();    

        if (m_light)
            m_light->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
        
        getGameObject()->recomputeUpdateFlags();
        UpdateFlagsFromGameObject();

        if (getGameObject()->isEnabledInHierarchy() && isEnabled())
            OnEnable();
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::OnEnable()
    {
        super::OnEnable();
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::OnDisable()
    {
        super::OnDisable();
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::Update(const Context & _context)
    {
        if (m_light)
            RefreshGraphicInstance();
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_lightType"))
        {
            createLightDesc();
            createLight();
        }
        else if (_object == m_lightDesc)
        {
            createLight();
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::EnableComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        super::EnableComponentFlags(_flags, _enabled);

        if (m_light)
            m_light->SetInstanceFlags(InstanceFlags::Enabled, asBool(ComponentFlags::Enabled & GetComponentFlags()));
    }

    //--------------------------------------------------------------------------------------
    bool LightComponent::TryGetAABB(core::AABB & _aabb) const
    {
        if (m_light)
        {
            if (m_light->TryGetAABB(_aabb))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    renderer::IGraphicInstance * LightComponent::GetGraphicInstance()
    {
        return m_light;
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::RefreshGraphicInstance()
    {
        m_light->setGlobalMatrix(GetGameObject()->getGlobalMatrix());
    }
}