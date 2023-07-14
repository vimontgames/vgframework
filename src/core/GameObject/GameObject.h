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
        using super = IGameObject;

        const char *                                            getClassName            () const override { return "GameObject"; }
        static bool                                             registerClass           (IFactory & _factory);
        static bool                                             registerProperties      (IClassDesc & _desc);

                                                                GameObject              (const core::string & _name, IObject * _parent);
        virtual                                                 ~GameObject             ();

        void                                                    Update                  (double _dt);

        void                                                    AddComponent            (IComponent * _component) final;
        const vector<IComponent *> &                            GetComponents           () const final;

        void                                                    addComponent            (Component * _component);
        const vector<Component*> &                              getComponents           () const;

        void                                                    AddChild                (IGameObject * _gameObject) final;
        bool                                                    RemoveChild             (IGameObject * _gameObject) final;
        const vector<IGameObject*> &                            GetChildren             () const final;

        void                                                    AddGraphicInstance      (graphics::renderer::IGraphicInstance * _graphicInstance) final;
        void                                                    RemoveGraphicInstance   (graphics::renderer::IGraphicInstance * _graphicInstance) final;
        const vector<graphics::renderer::IGraphicInstance*> &   GetGraphicInstances     () const final;

        VG_INLINE const vector<GameObject*> &                   getChildren             () const { return m_children;}

    private:
        vector<Component*>                                      m_components;
        vector<GameObject*>                                     m_children;
        vector<graphics::renderer::IGraphicInstance*>           m_graphicInstances;
    };
}