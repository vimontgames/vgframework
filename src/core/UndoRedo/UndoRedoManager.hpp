#include "core/Precomp.h"
#include "UndoRedoManager.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoManager::UndoRedoManager()
    {

    }

    //--------------------------------------------------------------------------------------
    UndoRedoManager::~UndoRedoManager()
    {
        Clear();
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::Undo()
    {
        if (HasUndo())
        {
            //VG_INFO("[Undo/Redo] Undo");

            if (m_doneStack.empty())
                return false;

            UndoRedoEntry * entry = m_doneStack.back();
            m_doneStack.pop_back();

            entry->Undo();
            m_undoneStack.push_back(entry);
            return true;
        }

        VG_INFO("[Undo/Redo] Nothing to Undo");
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::Redo()
    {
        if (HasRedo())
        {
            //VG_INFO("[Undo/Redo] Redo");

            UndoRedoEntry * entry = m_undoneStack.back();
            m_undoneStack.pop_back();

            entry->Redo();
            m_doneStack.push_back(entry);

            return true;
        }

        VG_INFO("[Undo/Redo] Nothing to Redo");
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::HasUndo() const
    {
        return !m_doneStack.empty();
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::HasRedo() const
    {
        return !m_undoneStack.empty();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::Clear()
    {
        for (UndoRedoEntry * cmd : m_doneStack)
            destroyEntry(cmd);
        m_doneStack.clear();

        for (UndoRedoEntry * cmd : m_undoneStack)
            destroyEntry(cmd);
        m_undoneStack.clear();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::BeforeChange(IUndoRedoEntry * _entry)
    {
        VG_ASSERT(nullptr != _entry);
        if (_entry)
        {
            m_currentEntry = _entry;
            _entry->BeforeChange();
        }
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::AfterChange()
    {
        VG_ASSERT(HasCurrentlyEditedEntry());
        if (auto * entry = GetEditedEntry())
        {
            entry->AfterChange();
            m_doneStack.push_back((UndoRedoEntry *)entry);
            for (UndoRedoEntry * e : m_undoneStack)
                destroyEntry(e);
            m_undoneStack.clear();
        }
    }

    //--------------------------------------------------------------------------------------
    const UndoRedoTarget & UndoRedoManager::GetCurrentUndoRedoTarget() const
    {
        return m_currentTarget;
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::SetCurrentUndoRedoTarget(const UndoRedoTarget & _undoRedoTarget)
    {
        VG_ASSERT(m_currentTarget.isEmpty());
        VG_ASSERT(!_undoRedoTarget.isEmpty());
        m_currentTarget = _undoRedoTarget;
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::ClearCurrentUndoRedoTarget()
    {
        m_currentTarget.clear();
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::HasCurrentlyEditedEntry() const
    {
        return nullptr != m_currentEntry;
    }

    //--------------------------------------------------------------------------------------
    IUndoRedoEntry * UndoRedoManager::GetEditedEntry() const
    {
        VG_ASSERT(HasCurrentlyEditedEntry());
        return m_currentEntry;
    }

    //--------------------------------------------------------------------------------------
    //void UndoRedoManager::createEntry(UndoRedoEntry * _entry)
    //{
    //    //return ma
    //}

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::destroyEntry(UndoRedoEntry * _entry)
    {
        if (_entry)
            delete _entry;
    }
}