#pragma once

#include "core/ISelection.h"

namespace vg::engine
{
    class Selection : public core::ISelection
    {
    public:
        core::IObject *                                 GetSelectedObject                   () final override;
        core::vector<core::IObject *> &                 GetSelectedObjects                  () final override;

        void                                            SetSelectedObject                   (core::IObject * _object) final override;
        void                                            SetSelectedObjects                  (const core::vector<core::IObject *> & _objects) final override;

        bool                                            IsSelectedObject                    (const core::IObject * _object) const final override;

        bool                                            Add                                 (core::IObject * _object) final override;
        bool                                            Remove                              (core::IObject * _object) final override;

        void                                            Clear                               () final override;

        core::float4x4 &                                GetSelectionMatrix                  () final override;
        core::vector<core::IGameObject *>               RemoveChildGameObjectsWithParents   (const core::vector<core::IObject *> & _objects) const final override;
        core::vector<core::IGameObject *>               DuplicateGameObjects                (const core::vector<core::IGameObject *> & _gameObjects) final override;

        bool                                            RegisterSelectionChangedCallback    (core::IObject * _this, OnSelectionChangedCallback _onSelectionChangedCallback) final override;
        bool                                            UnregisterSelectionChangedCallback  (core::IObject * _this, OnSelectionChangedCallback _onSelectionChangedCallback) final override;

    protected:
        bool                                            add                                 (core::IObject * _object);
        bool                                            remove                              (core::IObject * _object);
        void                                            setSelected                         (core::IObject * _object, bool _selected);
        void                                            clear                               ();
        void                                            updateSelectionMatrix               ();
        bool                                            hasAnySelectedAncestor              (const core::IObject * _object) const;
        void                                            updateSelectionChangedListeners     (core::SelectionChangeType _selectionChangeType);

    private:
        core::vector<core::IObject *>                   m_selectionArray;
        core::set<core::IObject *>                      m_selectionSet;
        core::float4x4                                  m_matrix = core::float4x4::identity();

        struct ListenerInfo
        {
            core::IObject *             m_listener = nullptr;
            OnSelectionChangedCallback  m_onSelectionChangedCallback = nullptr;
        };
        vg::core::vector<ListenerInfo>                  m_listeners;
    };
}