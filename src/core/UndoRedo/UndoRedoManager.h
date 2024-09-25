#pragma once

#include "core/IUndoRedo.h"
#include "UndoRedoEntry.h"

namespace vg::core
{
    class IObject;
    class IProperty;
    class IDynamicProperty;

    class UndoRedoManager final : public IUndoRedoManager
    {
    public:
        UndoRedoManager();
        ~UndoRedoManager();

        bool Undo() final override;
        bool Redo() final override;

        bool HasUndo() const final override;
        bool HasRedo() const final override;

        void Clear() final override;

        void BeforeChange(IUndoRedoEntry * _entry) final override;
        void AfterChange() final override;

        const UndoRedoTarget & GetCurrentUndoRedoTarget() const final override;
        void SetCurrentUndoRedoTarget(const UndoRedoTarget & _undoRedoTarget) final override;
        void ClearCurrentUndoRedoTarget() final override;

        bool HasCurrentlyEditedEntry() const final override;
        IUndoRedoEntry * GetEditedEntry() const final override;

    protected:
        void destroyEntry(UndoRedoEntry * _entry);

    private:
        core::vector<UndoRedoEntry *>   m_doneStack;
        core::vector<UndoRedoEntry *>   m_undoneStack;
        UndoRedoTarget                  m_currentTarget;
        core::IUndoRedoEntry *          m_currentEntry = nullptr;
    };
}