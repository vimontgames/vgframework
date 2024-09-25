#pragma once

namespace vg::core
{
    class IObject;
    class IProperty;
    class IDynamicProperty;

    class UndoRedoEntry : public IUndoRedoEntry
    {
    public:
        UndoRedoEntry() {}
        ~UndoRedoEntry() {}

        void BeforeChange() = 0;
        void AfterChange() = 0;

        void Undo() = 0;
        void Redo() = 0;
    };
}