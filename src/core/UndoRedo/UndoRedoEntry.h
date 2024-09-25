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

        void BeforeChange() override = 0;
        void AfterChange() override = 0;

        void Undo() override= 0;
        void Redo() override= 0;

        string GetName() const override = 0;
        string GetDescription() const override = 0;
    };
}