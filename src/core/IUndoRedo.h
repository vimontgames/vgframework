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
        virtual void AfterChange(IUndoRedoEntry * _entry) = 0;
    };
}