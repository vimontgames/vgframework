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

        void                                                Update                  (double _dt);

        void                                                AddComponent            (IComponent * _component) final override;
        IComponent *                                        AddComponent            (const char * _className, const core::string & _name) final override;
        bool                                                RemoveComponent         (IComponent * _component) final override;
        const vector<IComponent *> &                        GetComponents           () const final override;

        IComponent *                                        GetComponentByType      (const char * _className) const final override;

        void                                                AddChild                (IGameObject * _gameObject) final override;
        bool                                                RemoveChild             (IGameObject * _gameObject) final override;
        const vector<IGameObject*> &                        GetChildren             () const final override;
        bool                                                IsRoot                  () const final override;

        void                                                addComponent            (Component * _component);
        const vector<Component*> &                          getComponents           () const;

        void                                                addGraphicInstance      (renderer::IGraphicInstance * _graphicInstance);
        void                                                removeGraphicInstance   (renderer::IGraphicInstance * _graphicInstance);
        const vector<renderer::IGraphicInstance*> &         getGraphicInstances     () const;

        VG_INLINE const vector<GameObject*> &               getChildren             () const { return m_children;}

    private:
        vector<Component *>                                 m_components;
        vector<GameObject *>                                m_children;
        vector<renderer::IGraphicInstance*>                 m_graphicInstances;
    };
}