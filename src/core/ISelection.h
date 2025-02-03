#pragma once

#include "core/Object/Object.h"

vg_enum_class_ns(vg::core, SelectionChangeType, core::u8,
    Add,
    Remove,
    Clear
);

namespace vg::core
{
    class IGameObject;

    class ISelectionChangeListener
    {

    };

    class ISelection : public core::Object
    {
    public:
        using OnSelectionChangedCallback = void(__cdecl *)(core::IObject * _this, SelectionChangeType _selectionChangeType);

        virtual core::IObject *                     GetSelectedObject                   () = 0;
        virtual core::vector<core::IObject *> &     GetSelectedObjects                  () = 0;
        
        virtual void                                SetSelectedObject                   (core::IObject * _object) = 0;

        virtual void                                SetSelectedObjects                  (const core::vector<core::IObject *> & _objects) = 0;
        virtual bool                                IsSelectedObject                    (const core::IObject * _object) const = 0;
        
        virtual bool                                Add                                 (core::IObject * _object) = 0;
        virtual bool                                Remove                              (core::IObject * _object) = 0;

        virtual void                                Clear                               () = 0;

        virtual core::float4x4 &                    GetSelectionMatrix                  () = 0;
        virtual core::vector<core::IGameObject *>   RemoveChildGameObjectsWithParents   (const core::vector<core::IObject *> & _objects) const = 0;
        virtual core::vector<core::IGameObject*>    DuplicateGameObjects                (const core::vector<core::IGameObject *> & _gameObjects) = 0;

        virtual bool                                RegisterSelectionChangedCallback    (core::IObject * _this, OnSelectionChangedCallback _onSelectionChangedCallback) = 0;
        virtual bool                                UnregisterSelectionChangedCallback  (core::IObject * _this, OnSelectionChangedCallback _onSelectionChangedCallback) = 0;
    };
}