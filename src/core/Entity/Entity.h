#pragma once

#include "core/IEntity.h"

#define VG_AUTO_REGISTER_ENTITY(e) VG_AUTO_REGISTER_CLASS_EX(e, vg::core::IClassDesc::Flags::Entity)

namespace vg::core
{
    class Component;
    class IFactory;
    class IClassDesc;

    class Entity : public IEntity
    {
    public:
        using super = IEntity;

        const char *                getClassName        () const override { return "Entity"; }
        static bool                 registerClass       (IFactory & _factory);
        static bool                 registerProperties  (IClassDesc & _desc);

                                    Entity              (const core::string & _name, IObject * _parent);
        virtual                     ~Entity             ();

        Flags                       getFlags            () const final;
        void                        setFlags            (Flags flags, bool enabled) final;

        virtual void                update              (double _dt);
        ISector *                   getSector           () const final;

        void                        addComponent        (Component * _component);
        const vector<Component*> &  getComponents       () const;

    private:
        Flags                       m_flags;
        float4                      m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
        vector<Component*>          m_components;
    };
}