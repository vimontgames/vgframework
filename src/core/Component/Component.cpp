#include "core/Precomp.h"
#include "Component.h"
#include "core/IProperty.h"
#include "core/IEntity.h"
#include "core/IFactory.h"
#include "core/IClassDesc.h"
#include "core/Object/EnumHelper.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Component::Component(const core::string & _name, IObject * _parent) :
        IComponent(_name, _parent),
        m_flags(Flags::Enabled)
    {
        VG_ASSERT(dynamic_cast<IEntity*>(_parent));
    }

    //--------------------------------------------------------------------------------------
    Component::~Component()
    {
        
    }

    //--------------------------------------------------------------------------------------
    const IEntity * Component:: getEntity() const 
    { 
        return (const IEntity*)getParent();
    }

    //--------------------------------------------------------------------------------------
    bool Component::registerProperties(IClassDesc & _desc)
    {
        EnumHelper<Flags> flagsEnum;
        _desc.registerPropertyEnumHelper(Component, Flags, m_flags, "Flags", flagsEnum.getCount(), flagsEnum.getNames(), flagsEnum.getValues(), IProperty::Flags::Bitfield);

        return true;
    }

    //--------------------------------------------------------------------------------------
    IComponent::Flags Component::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    void Component::setFlags(Flags flags, bool enabled)
    {
        if (enabled)
            m_flags |= flags;
        else
            (u32&)m_flags &= ~(u32)flags;
    }
}