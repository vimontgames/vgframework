#pragma once

namespace vg::core
{
    class IObject;
    class IProperty;
    class IDynamicProperty;

    class UndoRedoEntry : public IUndoRedoEntry
    {
    public:
        UndoRedoEntry(IObject * _object);
        ~UndoRedoEntry();

        void BeforeChange() override = 0;
        void AfterChange() override = 0;

        void Undo() override= 0;
        void Redo() override= 0;

        string GetEntryName() const override = 0;
        string GetDescription() const override = 0;

        void AddSubEntry(IUndoRedoEntry * _subEntry) override { VG_ASSERT(false); }
        const vector<IUndoRedoEntry *> * GetSubEntries() const override { return nullptr; }

        IObject * GetObject() const override;
        string GetObjectName() const override;

    protected:
        UID m_objectUID;
        string m_objectName;
    };
}