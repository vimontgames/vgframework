#pragma once

#include "core/IEntity.h"
#include "core/Component/Component.h"

namespace vg::core
{
    class IObjectFactory;
    class IObjectDescriptor;

    class Entity : public IEntity
    {
    public:
        using super = IEntity;

        const char * getClassName() const override { return "Entity"; }
        static bool registerClass(IObjectFactory & _factory);
        static bool registerProperties(IObjectDescriptor & _desc);

        Entity(const core::string & _name = "");
        virtual ~Entity();

        virtual void update(double _dt);

        void setMatrix(const float4x4 _mat44);
        const float4x4 & getMatrix() const;

        void addComponent(Component * _component);
        const vector<Component*> & getComponents() const;

    private:
        float4x4            m_matrix = float4x4::identity();
        vector<Component*>  m_components;
    };
}