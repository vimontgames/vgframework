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
        virtual                                                 ~GameObject             ();

        void                                                    Update                  (double _dt);

        void                                                    AddComponent            (IComponent * _component) final override;
        const vector<IComponent *> &                            GetComponents           () const final override;

        void                                                    addComponent            (Component * _component);
        Component *                                             addComponent            (const char * _className, const core::string & _name);
        template <class T> T *                                  addComponent            (const core::string & _name);
        const vector<Component*> &                              getComponents           () const;

        Component *                                             findComponentByType     (const char * _className) const;
        template <class T> T *                                  findComponent           () const;

        void                                                    AddChild                (IGameObject * _gameObject) final;
        bool                                                    RemoveChild             (IGameObject * _gameObject) final;
        const vector<IGameObject*> &                            GetChildren             () const final;

        void                                                    AddGraphicInstance      (graphics::renderer::IGraphicInstance * _graphicInstance) final;
        void                                                    RemoveGraphicInstance   (graphics::renderer::IGraphicInstance * _graphicInstance) final;
        const vector<graphics::renderer::IGraphicInstance*> &   GetGraphicInstances     () const final;

        VG_INLINE const vector<GameObject*> &                   getChildren             () const { return m_children;}

    private:
        vector<Component *>                                     m_components;
        vector<GameObject *>                                    m_children;
        vector<graphics::renderer::IGraphicInstance*>           m_graphicInstances;
    };

    //--------------------------------------------------------------------------------------
    template <class T> T * GameObject::findComponent() const
    {
        return static_cast<T *>(findComponentByType(T::getStaticClassName()));
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * GameObject::addComponent(const core::string & _name)
    {
        return static_cast<T *>(addComponent(T::getStaticClassName(), _name));
    }
}