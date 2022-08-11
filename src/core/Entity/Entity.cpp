#include "core/Precomp.h"
#include "Entity.h"
#include "core/Component/Component.h"
#include "core/Object/AutoRegisterClass.h"  
#include "core/Object/EnumHelper.h"

namespace vg::core
{
    VG_AUTO_REGISTER_CLASS(Entity);

    //--------------------------------------------------------------------------------------
    bool Entity::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Entity, "Entity", IClassDesc::Flags::Entity))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Entity::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        EnumHelper<Flags> flagsEnum;
        _desc.registerPropertyEnumHelper(Entity, Flags, m_flags, "Flags", flagsEnum.getCount(), flagsEnum.getNames(), flagsEnum.getValues(), IProperty::Flags::Bitfield);

        _desc.registerPropertyHelper(Entity, m_color, "Color", IProperty::Flags::Color);
        _desc.registerPropertyObjectPointerVectorHelper(Entity, m_components, "Components", IProperty::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Entity::Entity(const core::string & _name, IObject * _parent) :
        IEntity(_name, _parent),
        m_flags(Flags::Enabled)
    {

    }

    //--------------------------------------------------------------------------------------
    Entity::~Entity()
    {
        for (uint i = 0; i < m_components.size(); ++i)
            VG_SAFE_RELEASE(m_components[i]);

        m_components.clear();
    }

    //--------------------------------------------------------------------------------------
    IEntity::Flags Entity::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    void Entity::setFlags(Flags flags, bool enabled)
    {
        if (enabled)
            m_flags |= flags;
        else
            (u32&)m_flags &= ~(u32)flags;
    }

    //--------------------------------------------------------------------------------------
    void Entity::update(double _dt)
    {
        for (uint i = 0; i < m_components.size(); ++i)
        {
            Component * component = m_components[i];
            component->update(_dt);
        }
    }

    //--------------------------------------------------------------------------------------
    ISector * Entity::getSector() const
    {
        return (ISector*)getParent();
    }

    //--------------------------------------------------------------------------------------
    void Entity::addComponent(Component * _component)
    {
        VG_ASSERT(_component);
        _component->addRef();
        m_components.push_back(_component);
    }

    //--------------------------------------------------------------------------------------
    const vector<Component*> &  Entity::getComponents() const
    {
        return m_components;
    }
}