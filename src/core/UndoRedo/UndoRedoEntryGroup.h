#pragma once

namespace vg::core
{
    class IObject;
    class IProperty;
    class IDynamicProperty;

    class UndoRedoEntryGroup : public IUndoRedoEntry
    {
    public:
        UndoRedoEntryGroup(const string & _name);
        ~UndoRedoEntryGroup();

        void BeforeChange() override;
        void AfterChange() override;

        void Undo() override;
        void Redo() override;

        string GetEntryName() const override;
        string GetObjectName() const final override;
        string GetDescription() const override;

        void AddSubEntry(IUndoRedoEntry * _subEntry) final override;
        const vector<IUndoRedoEntry *> * GetSubEntries() const final override;

    private:
        string m_name;
        core::vector<IUndoRedoEntry *> m_subEntries;
    };
}