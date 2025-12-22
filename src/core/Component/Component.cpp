#include "core/Precomp.h"
#include "Component.h"
#include "core/IProperty.h"
#include "core/IGameObject.h"
#include "core/IFactory.h"
#include "core/IClassDesc.h"
#include "core/Kernel.h"
#include "core/Object/EnumHelper.h"
#include "core/GameObject/GameObject.h"

#if !VG_ENABLE_INLINE
#include "Component.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Component::Component(const core::string & _name, IObject * _parent) :
        IComponent(_name, _parent),
        m_flags(ComponentFlags::Enabled),
        m_update(UpdateFlags::Update)
    {
        VG_ASSERT(_name.empty() || nullptr != dynamic_cast<IGameObject*>(_parent), "Creating Component without parent is only allowed during static initialization");
    }

    //--------------------------------------------------------------------------------------
    Component::~Component()
    {
 
    }

    //--------------------------------------------------------------------------------------
    void Component::OnPlay()
    {

    }

    //--------------------------------------------------------------------------------------
    void Component::OnStop()
    {

    }

    //--------------------------------------------------------------------------------------
    void Component::OnEnable()
    {
        if (GameObject * go = (GameObject *)GetGameObject())
            go->recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    void Component::OnDisable()
    {
        if (GameObject * go = (GameObject *)GetGameObject())
            go->recomputeUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    void Component::FixedUpdate(const Context & _context)
    {
        VG_ASSERT(false, "Component \"%s\" from GameObject \"%s\" should implement 'FixedUpdate'", GetClassName(), _context.m_gameObject->GetName().c_str());
    }

    //--------------------------------------------------------------------------------------
    void Component::Update(const Context & _context)
    {
        VG_ASSERT(false, "Component \"%s\" from GameObject \"%s\" should implement 'Update'", GetClassName(), _context.m_gameObject->GetName().c_str());
    }

    //--------------------------------------------------------------------------------------
    void Component::LateUpdate(const Context & _context)
    {
        VG_ASSERT(false, "Component \"%s\" from GameObject \"%s\" should implement 'LateUpdate'", GetClassName(), _context.m_gameObject->GetName().c_str());
    }

    //--------------------------------------------------------------------------------------
    const IGameObject * Component::GetGameObject() const 
    { 
        return (const IGameObject*)getGameObject();
    }

    //--------------------------------------------------------------------------------------
    IGameObject * Component::GetGameObject()
    {
        return (IGameObject *)getGameObject();
    }

    //--------------------------------------------------------------------------------------
    bool Component::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
      
        registerPropertyEnumBitfield(Component, ComponentFlags, m_flags, "Flags");
        setPropertyDescription(Component, m_flags, "Component flags");

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Component::Enable(bool _enable) 
    {
        EnableComponentFlags(ComponentFlags::Enabled, _enable);
    }

    //--------------------------------------------------------------------------------------
    ComponentFlags Component::GetComponentFlags() const
    {
        return getComponentFlags();
    }

    //--------------------------------------------------------------------------------------
    void Component::EnableComponentFlags(ComponentFlags _flags, bool _enabled)
    {
        setComponentFlags(_flags, _enabled);

        if (asBool(ComponentFlags::Enabled & _flags))
        {
            if (_enabled)
                OnEnable();
            else
                OnDisable();  
        }
    }

    //--------------------------------------------------------------------------------------
    UpdateFlags Component::GetUpdateFlags() const
    {
        return getUpdateFlags();
    }

    //--------------------------------------------------------------------------------------
    void Component::EnableUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        enableUpdateFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    const core::string Component::GetSubObjectName(core::uint _subObjectIndex) const 
    { 
        return GetClassName(); 
    };

    //--------------------------------------------------------------------------------------
    void Component::SetPropertyValue(const IProperty & _prop, void * _previousValue, void * _newValue)
    {
        if (&m_flags == _previousValue)
        {
            auto newFlags = *(ComponentFlags *)_newValue;

            if ((ComponentFlags::Enabled & m_flags) != (ComponentFlags::Enabled & newFlags))
            {
                if (asBool(ComponentFlags::Enabled & newFlags))
                    OnEnable();
                else
                    OnDisable();
            }
        }

        super::SetPropertyValue(_prop, _previousValue, _newValue);
    }
}