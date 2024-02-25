#pragma once

#include "core/IGameObject.h"

namespace vg::core
{
    class Component;
    class IFactory;
    class IClassDesc;

    class GameObject : public IGameObject
    {
    public:
        VG_CLASS_DECL(GameObject, IGameObject);

                                                            GameObject              (const core::string & _name, IObject * _parent);
        virtual                                             ~GameObject             ();

        void                                                OnLoad                  () override;

        void                                                OnPlay                  () override;
        void                                                OnStop                  () override;

        void                                                setParent               (IObject * _parent) final override;

        UpdateFlags                                         GetUpdateFlags          () const final override;
        void                                                SetUpdateFlags          (UpdateFlags _flags, bool _enabled) final override;

        IBaseScene *                                        GetScene                () const final override;
        IWorld *                                            GetWorld                () const final override;

        bool                                                IsPrefab                () const override;
        IGameObject *                                       GetParentPrefab         () const override;
        IResource *                                         GetPrefabResource       () const override;

        void                                                FixedUpdate             (float _dt);
        void                                                Update                  (float _dt);
        void                                                LateUpdate              (float _dt);

        void                                                AddComponent            (IComponent * _component, core::uint _index = -1) final override;
        IComponent *                                        AddComponent            (const char * _className, const core::string & _name) final override;
        bool                                                RemoveComponent         (IComponent * _component) final override;
        const vector<IComponent *> &                        GetComponents           () const final override;
        core::uint                                          GetComponentIndex       (const IComponent * _component) const;

        IComponent *                                        GetComponentByType      (const char * _className) const final override;

        void                                                AddChild                (IGameObject * _gameObject, core::uint _index = -1) final override;
        bool                                                RemoveChild             (IGameObject * _gameObject, bool _recomputeFlags = true) final override;
        const vector<IGameObject*> &                        GetChildren             () const override;
        bool                                                IsRoot                  () const final override;
        bool                                                HasAncestor             (const IGameObject * _ancestor) const final override;
        core::uint                                          GetChildIndex           (const IGameObject * _child) const final override;

        void                                                OnPropertyChanged       (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        void                                                addComponent            (Component * _component, core::uint _index = -1);
        const vector<Component*> &                          getComponents           () const;

        void                                                addGraphicInstance      (renderer::IGraphicInstance * _graphicInstance);
        void                                                removeGraphicInstance   (renderer::IGraphicInstance * _graphicInstance);
        const vector<renderer::IGraphicInstance*> &         getGraphicInstances     () const;

        VG_INLINE const vector<GameObject*> &               getChildren             () const;

        void                                                recomputeUpdateFlags    ();

        VG_INLINE UpdateFlags                               getUpdateFlags          () const;
        VG_INLINE void                                      setUpdateFlags          (UpdateFlags _flags, bool _enabled);

    private:
        UpdateFlags                                         m_update = (UpdateFlags)0x0;
        vector<Component *>                                 m_components;
        vector<GameObject *>                                m_children;
        vector<renderer::IGraphicInstance*>                 m_graphicInstances;
    };
}

#if VG_ENABLE_INLINE
#include "GameObject.inl"
#endif