#pragma once

#include "core/Object/Object.h"

// *VERY* experimental feature: Only supports undo/redo float/float2/float3/float4 properties for now and only from ImGui inspector
#define VG_ENABLE_UNDO_REDO 1

namespace vg::core
{
    class IUndoRedoEntry
    {
    public:
        IUndoRedoEntry() {}
        virtual ~IUndoRedoEntry() {}

        virtual void BeforeChange() = 0;
        virtual void AfterChange() = 0;

        virtual void Undo() = 0;
        virtual void Redo() = 0;

        virtual string GetEntryName() const = 0;
        virtual string GetObjectName() const = 0;
        virtual string GetDescription() const = 0;

        virtual void AddSubEntry(IUndoRedoEntry * _subEntry) = 0;
        virtual const vector<IUndoRedoEntry *> * GetSubEntries() const = 0;
    };

    struct UndoRedoTarget
    {
        UndoRedoTarget() :
            m_object(nullptr),
            m_prop(nullptr)
        {

        }

        UndoRedoTarget(IObject * _object, const IProperty * _prop) :
            m_object(_object),
            m_prop(_prop)
        {

        }

        bool operator != (const UndoRedoTarget & _other) const
        {
            return m_object != _other.m_object && m_prop != _other.m_prop;
        }

        bool isEmpty() const
        {
            return nullptr == m_object;// && nullptr == m_prop; 
        }

        void clear()
        {
            m_object = nullptr;
            m_prop = nullptr;
        }

        IObject *           m_object = nullptr;
        const IProperty *   m_prop = nullptr;
    };

    class IUndoRedoManager : public Object
    {
    public:
        IUndoRedoManager() {}
        virtual ~IUndoRedoManager() {}

        virtual bool Undo() = 0;
        virtual bool Redo() = 0;

        virtual bool HasUndo() const = 0;
        virtual bool HasRedo() const = 0;

        virtual void Clear() = 0;

        virtual void BeforeChange(IUndoRedoEntry * _entry) = 0;
        virtual void AfterChange() = 0;

        virtual const UndoRedoTarget & GetCurrentUndoRedoTarget() const = 0;
        virtual void SetCurrentUndoRedoTarget(const UndoRedoTarget & _undoRedoTarget) = 0;
        virtual void ClearCurrentUndoRedoTarget() = 0;

        virtual bool HasCurrentlyEditedEntry() const = 0;
        virtual IUndoRedoEntry * GetEditedEntry() const = 0;
        virtual bool ClearEditedEntry() = 0;

        virtual const core::vector<IUndoRedoEntry *> & GetUndoStack() const = 0;
        virtual const core::vector<IUndoRedoEntry *> & GetRedoStack() const = 0;
    };
}