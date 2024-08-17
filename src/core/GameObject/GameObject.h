#pragma once

#include "core/IGameObject.h"

namespace vg::core
{
    class Component;
    class IFactory;
    class IClassDesc;
    class IBehaviour;

    class GameObject : public IGameObject
    {
    public:
        VG_CLASS_DECL(GameObject, IGameObject);

                                                        GameObject                  (const core::string & _name, IObject * _parent);
        virtual                                         ~GameObject                 ();

        bool                                            RegisterUID               () override;

        void                                            OnLoad                      () override;

        void                                            OnPlay                      () override;
        void                                            OnStop                      () override;

        void                                            OnEnable                    () override;
        void                                            OnDisable                   () override;

        void                                            setParent                   (IObject * _parent) final override;

        void                                            SetInstanceFlags            (InstanceFlags _flags, bool _enabled) final override;

        UpdateFlags                                     GetUpdateFlags              () const final override;
        void                                            SetUpdateFlags              (UpdateFlags _flags, bool _enabled) final override;

        IBaseScene *                                    GetScene                    () const final override;
        IWorld *                                        GetWorld                    () const final override;

        bool                                            IsPrefab                    () const override;
        IGameObject *                                   GetParentPrefab             () const override;
        IResource *                                     GetPrefabResource           () const override;

        IDynamicPropertyList *                          GetDynamicPropertyList      (const IObject * _object) const override;
        IDynamicPropertyList *                          CreateDynamicPropertyList   (const IObject * _object) override;

        IDynamicProperty *                              GetDynamicProperty          (const IObject * _object, const IProperty * _prop) const override;
        bool                                            CanOverrideProperty         (const IObject * _object, const IProperty * _prop) const override;
        IDynamicProperty *                              CreateDynamicProperty       (const IObject * _object, const IProperty * _prop) override;
        void                                            OverrideGameObjectProperties(IGameObject * _gameObject, const IDynamicProperty * _dynProp) override;
        bool                                            ToggleOverride              (const IObject * _object, const IProperty * _prop, bool _override) override;

        using Context = GameObjectUpdateContext;

        void                                            FixedUpdate                 (const Context & _context);
        void                                            Update                      (const Context & _context);
        void                                            LateUpdate                  (const Context & _context);

        const core::vector<IBehaviour*> /*&*/           getBehaviours               () const;

        void                                            AddComponent                (IComponent * _component, uint _index = -1) final override;
        IComponent *                                    AddComponent                (const char * _className, const string & _name) final override;
        bool                                            RemoveComponent             (IComponent * _component) final override;
        const vector<IComponent *> &                    GetComponents               () const final override;
        core::uint                                      GetComponentIndex           (const IComponent * _component) const;

        IComponent *                                    GetComponentByType          (const char * _className, bool _searchInParent = false, bool _searchInChildren = false) const final override;
        vector<IComponent *>                            GetComponentsByType         (const char * _className, bool _searchInParent = false, bool _searchInChildren = false) const final override;

        // rem: have to forward them for C++ template name lookup to work, otherwise from GameObject we would have to use super:: or IGameObject:: to access these funcs
        template <class T> inline T *                   AddComponentT               (const string & _name)  { return super::AddComponentT<T>(_name); }
        template <class T> inline T *                   GetComponentT               () const                { return super::GetComponentT<T>(); }
        template <class T> inline T *                   GetComponentInParentsT      () const                { return super::GetComponentInParentsT<T>(); }
        template <class T> inline T *                   GetComponentInChildrenT     () const                { return super::GetComponentInChildrenT<T>(); }

        template <class T> inline core::vector<T *>     GetComponentsByTypeT        () const { return super::GetComponentsT<T>(); }
        template <class T> inline core::vector<T *>     GetComponentsInChildrenT    () const { return super::GetComponentsT<T>(); }

        void                                            AddChild                    (IGameObject * _gameObject, uint _index = -1) final override;
        bool                                            RemoveChild                 (IGameObject * _gameObject, bool _recomputeFlags = true) final override;
        core::uint                                      RemoveAllChildren           (bool _recomputeFlags = true) final override;
        const vector<IGameObject*> &                    GetChildren                 () const override;
        bool                                            IsRoot                      () const final override;
        bool                                            HasAncestor                 (const IGameObject * _ancestor) const final override;
        core::uint                                      GetChildIndex               (const IGameObject * _child) const final override;
        IGameObject *                                   GetChildGameObject          (const string & _name) const final override;

        bool                                            IsEnabledInHierarchy        () const final override;

        bool                                            TryGetAABB                  (AABB & _aabb) const override;

        void                                            OnPropertyChanged           (IObject * _object, const IProperty & _prop, bool _notifyParent) override;

        void                                            OnCollisionEnter            (IGameObject * _other) final override;
        void                                            OnCollisionStay             (IGameObject * _other) final override;
        void                                            OnCollisionExit             (IGameObject * _other) final override;
                                                                                                                
        void                                            OnTriggerEnter              (IGameObject * _other) final override;
        void                                            OnTriggerStay               (IGameObject * _other) final override;
        void                                            OnTriggerExit               (IGameObject * _other) final override;

        void                                            addComponent                (Component * _component, uint _index = -1);
        const vector<Component*> &                      getComponents               () const;

        void                                            addGraphicInstance          (renderer::IGraphicInstance * _graphicInstance);
        void                                            removeGraphicInstance       (renderer::IGraphicInstance * _graphicInstance);
        const vector<renderer::IGraphicInstance*> &     getGraphicInstances         () const;

        VG_INLINE const vector<GameObject*> &           getChildren                 () const;

        void                                            recomputeUpdateFlags        ();

        VG_INLINE UpdateFlags                           getUpdateFlags              () const;
        VG_INLINE void                                  setUpdateFlags              (UpdateFlags _flags, bool _enabled);

        void                                            sortComponents              ();      

    private:
        UpdateFlags                                     m_update = (UpdateFlags)0x0;
        vector<Component *>                             m_components;
        vector<GameObject *>                            m_children;
        vector<renderer::IGraphicInstance*>             m_graphicInstances;
    };
}

#if VG_ENABLE_INLINE
#include "GameObject.inl"
#endif