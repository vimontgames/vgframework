#pragma once

#include "core/Instance/Instance.h"

namespace vg::renderer
{
    class IGraphicInstance; // Should be part of vg::core?
}

namespace vg::core
{
    class IComponent;

    class IGameObject : public Instance
    {
    public:
        IGameObject(const core::string & _name, IObject * _parent) :
            Instance(_name, _parent)
        {

        }

        virtual void                                        AddChild                (IGameObject * _gameObject) = 0;
        virtual bool                                        RemoveChild             (IGameObject * _gameObject) = 0;
        virtual const vector<IGameObject*> &                GetChildren             () const = 0;
        virtual bool                                        IsRoot                  () const = 0;

        virtual void                                        AddComponent            (IComponent * _component) = 0;
        virtual const vector<IComponent *> &                GetComponents           () const = 0;

        virtual void                                        AddGraphicInstance      (renderer::IGraphicInstance * _graphicInstance) = 0;
        virtual void                                        RemoveGraphicInstance   (renderer::IGraphicInstance * _graphicInstance) = 0;
        virtual const vector<renderer::IGraphicInstance*> & GetGraphicInstances     () const = 0;
    };
}