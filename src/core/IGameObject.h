#pragma once

#include "core/Instance/Instance.h"
#include "core/Object/Update.h"

namespace vg::renderer
{
    class IGraphicInstance; // Should be part of vg::core?
}

namespace vg::core
{
    class IComponent;
    class IBaseScene;
    class IWorld;
    class IDynamicPropertyList;
    class IDynamicProperty;

    class IGameObject : public Instance
    {
    public:
        IGameObject(const core::string & _name, IObject * _parent) :
            Instance(_name, _parent)
        {

        }

        virtual void                            AddChild                        (IGameObject * _gameObject, core::uint _index = -1) = 0;
        virtual bool                            RemoveChild                     (IGameObject * _gameObject, bool _recomputeFlags = true) = 0;
        virtual core::uint                      RemoveAllChildren               (bool _recomputeFlags = true) = 0;
        virtual const vector<IGameObject*> &    GetChildren                     () const = 0;
        virtual bool                            IsRoot                          () const = 0;
        virtual bool                            HasAncestor                     (const IGameObject * _ancestor) const = 0;
        virtual core::uint                      GetChildIndex                   (const IGameObject * _child) const = 0;
        virtual IGameObject *                   GetChildGameObject              (const string & _name) const = 0;

        virtual bool                            IsEnabledInHierarchy            () const = 0;

        virtual void                            AddComponent                    (IComponent * _component, core::uint _index = -1) = 0;
        virtual IComponent *                    AddComponent                    (const char * _className, const core::string & _name) = 0;
        virtual bool                            RemoveComponent                 (IComponent * _component) = 0;
        virtual const vector<IComponent *> &    GetComponents                   () const = 0;
        virtual IComponent *                    GetComponentByType              (const char * _className, bool _searchInParent = false, bool _searchInChildren = false) const = 0;
        virtual vector<IComponent *>            GetComponentsByType             (const char * _className, bool _searchInParent = false, bool _searchInChildren = false) const = 0;
        virtual core::uint                      GetComponentIndex               (const IComponent * _component) const = 0;

        virtual UpdateFlags                     GetUpdateFlags                  () const = 0;
        virtual void                            SetUpdateFlags                  (UpdateFlags _flags, bool _enabled) = 0;

        virtual IBaseScene *                    GetScene                        () const = 0;
        virtual IWorld *                        GetWorld                        () const = 0;
        
        virtual bool                            IsPrefab                        () const = 0;
        virtual IGameObject *                   GetParentPrefab                 () const = 0;
        virtual IResource *                     GetPrefabResource               () const = 0;

        virtual IDynamicPropertyList *          GetDynamicPropertyList          (const IObject * _object) const = 0;
        virtual IDynamicPropertyList *          CreateDynamicPropertyList       (const IObject * _object) = 0;

        virtual IDynamicProperty *              GetDynamicProperty              (const IObject * _object, const IProperty * _prop) const = 0;
        virtual bool                            CanOverrideProperty             (const IObject * _object, const IProperty * _prop) const = 0;
        virtual IDynamicProperty *              CreateDynamicProperty           (const IObject * _object, const IProperty * _prop) = 0;
        virtual void                            OverrideGameObjectProperties    (IGameObject * _gameObject, const IDynamicProperty * _dynProp) = 0;
        virtual bool                            ToggleOverride                  (const IObject * _object, const IProperty * _prop, bool _override) = 0;

        virtual void                            OnCollisionEnter                (IGameObject * _other) = 0;
        virtual void                            OnCollisionStay                 (IGameObject * _other) = 0;
        virtual void                            OnCollisionExit                 (IGameObject * _other) = 0;
                                                                                       
        virtual void                            OnTriggerEnter                  (IGameObject * _other) = 0;
        virtual void                            OnTriggerStay                   (IGameObject * _other) = 0;
        virtual void                            OnTriggerExit                   (IGameObject * _other) = 0;

        template <class T> T *                  AddComponentByType              (const string & _name);
        template <class T> T *                  GetComponentByType              () const;
        template <class T> core::vector<T *>    GetComponentsByType             () const;
        template <class T> T *                  GetComponentInParents           () const;
        template <class T> T *                  GetComponentInChildren          () const;
    };

    //--------------------------------------------------------------------------------------
    template <class T> T * IGameObject::AddComponentByType(const core::string & _name)
    {
        return static_cast<T *>(AddComponent(T::getStaticClassName(), _name));
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * IGameObject::GetComponentByType() const
    {
        return static_cast<T *>(GetComponentByType(T::getStaticClassName(), false, false));
    }

    //--------------------------------------------------------------------------------------
    template <class T> core::vector<T*> IGameObject::GetComponentsByType() const
    {
        return (core::vector<T*>&)(GetComponentsByType(T::getStaticClassName(), false, false));
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * IGameObject::GetComponentInParents() const
    {
        return static_cast<T *>(GetComponentByType(T::getStaticClassName(), true, false));
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * IGameObject::GetComponentInChildren() const
    {
        return static_cast<T *>(GetComponentByType(T::getStaticClassName(), false, true));
    }
}