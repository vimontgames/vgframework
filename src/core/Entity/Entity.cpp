#include "core/Precomp.h"
#include "Entity.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Entity::Entity(const core::string & _name) :
        Object(_name)
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
            component->update(this, _dt);
        }
    }

    //--------------------------------------------------------------------------------------
    void Entity::setMatrix(const float4x4 _mat44)
    {
        m_matrix = _mat44;
    }

    //--------------------------------------------------------------------------------------
    const float4x4 & Entity::getMatrix() const
    {
        return m_matrix;
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