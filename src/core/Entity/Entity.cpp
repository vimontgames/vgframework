#include "core/Precomp.h"
#include "Entity.h"
#include "core/Component/Component.h"
#include "core/Object/AutoRegisterClass.h"  

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

        _desc.registerPropertyObjectPointerVectorHelper(Entity, m_components, "Components", IProperty::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Entity::Entity(const core::string & _name, IObject * _parent) :
        IEntity(_name, _parent)
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