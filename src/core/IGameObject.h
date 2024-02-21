#pragma once

#include "core/Instance/Instance.h"
#include "core/Object/UpdateFlags.h"

namespace vg::renderer
{
    class IGraphicInstance; // Should be part of vg::core?
}

namespace vg::core
{
    class IComponent;
    class IBaseScene;
    class IWorld;

    class IGameObject : public Instance
    {
    public:
        IGameObject(const core::string & _name, IObject * _parent) :
            Instance(_name, _parent)
        {

        }

        virtual void                            AddChild            (IGameObject * _gameObject, core::uint _index = -1) = 0;
        virtual bool                            RemoveChild         (IGameObject * _gameObject) = 0;
        virtual const vector<IGameObject*> &    GetChildren         () const = 0;
        virtual bool                            IsRoot              () const = 0;
        virtual bool                            HasAncestor         (const IGameObject * _ancestor) const = 0;
        virtual core::uint                      GetChildIndex       (const IGameObject * _child) const = 0;

        virtual void                            AddComponent        (IComponent * _component, core::uint _index = -1) = 0;
        virtual IComponent *                    AddComponent        (const char * _className, const core::string & _name) = 0;
        virtual bool                            RemoveComponent     (IComponent * _component) = 0;
        virtual const vector<IComponent *> &    GetComponents       () const = 0;
        virtual IComponent *                    GetComponentByType  (const char * _className) const = 0;
        virtual core::uint                      GetComponentIndex   (const IComponent * _component) const = 0;

        virtual UpdateFlags                     GetUpdateFlags      () const = 0;
        virtual void                            SetUpdateFlags      (UpdateFlags _flags, bool _enabled) = 0;

        virtual IBaseScene *                    GetScene            () const = 0;
        virtual IWorld *                        GetWorld            () const = 0;
        
        virtual bool                            IsPrefab            () const = 0;
        virtual IResource *                     GetPrefabResource   () const = 0;

        template <class T> T *                  AddComponentByType  (const core::string & _name);
        template <class T> T *                  GetComponentByType  () const;
    };

    //--------------------------------------------------------------------------------------
    template <class T> T * IGameObject::GetComponentByType() const
    {
        return static_cast<T *>(GetComponentByType(T::getStaticClassName()));
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * IGameObject::AddComponentByType(const core::string & _name)
    {
        return static_cast<T *>(AddComponent(T::getStaticClassName(), _name));
    }
}