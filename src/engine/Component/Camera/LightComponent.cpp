#include "engine/Precomp.h"
#include "LightComponent.h"
#include "editor/Editor_Consts.h"
#include "renderer/ILightInstance.h"
#include "core/GameObject/GameObject.h"
#include "engine/Engine.h"
#include "renderer/IRenderer.h"

#if !VG_ENABLE_INLINE
#include "LightComponent.inl"
#endif

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(LightComponent, "Light", "Rendering", "Light component to light up scene", editor::style::icon::Light);

    //--------------------------------------------------------------------------------------
    bool LightComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(LightComponent, renderer::LightType, m_lightType, "Type");
        registerPropertyObjectPtrEx(LightComponent, m_lightDesc, "Light", IProperty::Flags::Flatten);

        return true;
    }

    //--------------------------------------------------------------------------------------
    LightComponent::LightComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_lightType(renderer::LightType::Omni)
    {
        SetUpdateFlags((UpdateFlags)0x0);

        if (nullptr == m_lightDesc)
            createLightDesc();
        if (nullptr == m_light)
            createLight();
    }

    //--------------------------------------------------------------------------------------
    LightComponent::~LightComponent()
    {
        VG_SAFE_RELEASE(m_lightDesc);

        if (m_registered)
        {
            getGameObject()->removeGraphicInstance(m_light);
            m_registered = false;
        }

        VG_SAFE_RELEASE(m_light);
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
                m_lightDesc = (renderer::ILightDesc *)factory->createObject("DirectionalLightDesc", "", this);
                break;

            case renderer::LightType::Omni:
                m_lightDesc = (renderer::ILightDesc *)factory->createObject("OmniLightDesc", "", this);
                break;
        }

        return nullptr != m_lightDesc;
    }

    //--------------------------------------------------------------------------------------
    bool LightComponent::createLight()
    {
        IFactory * factory = Kernel::getFactory();

        if (m_registered)
        {
            getGameObject()->removeGraphicInstance(m_light);
            m_registered = false;
        }

        VG_SAFE_RELEASE(m_light);

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
            getGameObject()->addGraphicInstance(m_light);
            m_registered = true;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::OnLoad()
    {
        createLight();

        if (m_light)
            m_light->SetFlags(IInstance::Flags::Enabled, asBool(IComponent::Flags::Enabled & GetFlags()));
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::Update(float _dt)
    {
        if (m_light)
            m_light->setGlobalMatrix(getGameObject()->getGlobalMatrix());
    }

    //--------------------------------------------------------------------------------------
    void LightComponent::OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.getName(), "m_lightType"))
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
    void LightComponent::SetFlags(IComponent::Flags _flags, bool _enabled)
    {
        super::SetFlags(_flags, _enabled);

        if (m_light)
            m_light->SetFlags(IInstance::Flags::Enabled, asBool(IComponent::Flags::Enabled & GetFlags()));
    }
}