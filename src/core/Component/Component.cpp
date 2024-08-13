#include "core/Precomp.h"
#include "Component.h"
#include "core/IProperty.h"
#include "core/IGameObject.h"
#include "core/IFactory.h"
#include "core/IClassDesc.h"
#include "core/Kernel.h"
#include "core/Object/EnumHelper.h"

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
    void Component::FixedUpdate(float _dt)
    {
        VG_ASSERT(false, "Component '%s' does not implement 'FixedUpdate'", GetClassName());
    }

    //--------------------------------------------------------------------------------------
    void Component::Update(float _dt) 
    {
        VG_ASSERT(false, "Component '%s' does not implement 'Update'", GetClassName());
    }

    //--------------------------------------------------------------------------------------
    void Component::LateUpdate(float _dt) 
    {
        VG_ASSERT(false, "Component '%s' does not implement 'LateUpdate'", GetClassName());
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
    ComponentFlags Component::GetComponentFlags() const
    {
        return getComponentFlags();
    }

    //--------------------------------------------------------------------------------------
    void Component::SetComponentFlags(ComponentFlags _flags, bool _enabled)
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
    void Component::SetUpdateFlags(UpdateFlags _flags, bool _enabled)
    {
        setUpdateFlags(_flags, _enabled);
    }

    //--------------------------------------------------------------------------------------
    const core::string Component::GetSubObjectName(core::uint _subObjectIndex) const 
    { 
        VG_ASSERT_NOT_IMPLEMENTED(); 
        return ""; 
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