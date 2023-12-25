#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IGameObject;

    class ISelection : public core::Object
    {
    public:
        virtual core::IObject *                     GetSelectedObject                   () = 0;
        virtual core::vector<core::IObject *> &     GetSelectedObjects                  () = 0;
        
        virtual void                                SetSelectedObject                   (core::IObject * _object) = 0;

        virtual void                                SetSelectedObjects                  (const core::vector<core::IObject *> & _objects) = 0;
        virtual bool                                IsSelectedObject                    (core::IObject * _object) = 0;
        
        virtual bool                                Add                                 (core::IObject * _object) = 0;
        virtual bool                                Remove                              (core::IObject * _object) = 0;

        virtual void                                Clear                               () = 0;

        virtual core::float4x4 &                    GetSelectionMatrix                  () = 0;
        virtual core::vector<core::IGameObject *>   RemoveChildGameObjectsWithParents   (const core::vector<core::IObject *> & _objects) const = 0;
        virtual core::vector<core::IGameObject*>    DuplicateGameObjects                (const core::vector<core::IGameObject *> & _gameObjects) = 0;
    };
}