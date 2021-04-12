#pragma once

#include "core/IEntity.h"

#define VG_AUTO_REGISTER_ENTITY(e) VG_AUTO_REGISTER_CLASS_EX(e, vg::core::IObjectDescriptor::Flags::Entity)

namespace vg::core
{
    class Component;
    class IObjectFactory;
    class IObjectDescriptor;

    class Entity : public IEntity
    {
    public:
        using super = IEntity;

        const char *                getClassName        () const override { return "Entity"; }
        static bool                 registerClass       (IObjectFactory & _factory);
        static bool                 registerProperties  (IObjectDescriptor & _desc);

                                    Entity              (const core::string & _name, IObject * _parent);
        virtual                     ~Entity             ();

        virtual void                update              (double _dt);

        ISector *                   getSector           () const final;

        void                        addComponent        (Component * _component);
        const vector<Component*> &  getComponents       () const;

    private:
        float4                      m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
        vector<Component*>          m_components;
    };
}