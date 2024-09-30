#include "UndoRedoEntryGroup.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoEntryGroup::UndoRedoEntryGroup(const string & _name) :
        UndoRedoEntry(nullptr),
        m_entryName(_name)
    {

    }

    //--------------------------------------------------------------------------------------
    UndoRedoEntryGroup::~UndoRedoEntryGroup()
    {
        for (auto * entry : m_subEntries)
            VG_SAFE_DELETE(entry);
        m_subEntries.clear();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoEntryGroup::BeforeChange()
    {
        for (auto * entry : m_subEntries)
            entry->BeforeChange();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoEntryGroup::AfterChange()
    {
        for (auto * entry : m_subEntries)
            entry->AfterChange();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoEntryGroup::Undo()
    {
        VG_INFO("[Undo/Redo] Begin Undo Group \"%s\" (0x%016X)", GetEntryName().c_str(), this);

        for (auto * entry : m_subEntries)
            entry->Undo();

        VG_INFO("[Undo/Redo] End Undo Group \"%s\" (0x%016X)", GetEntryName().c_str(), this);
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoEntryGroup::Redo()
    {
        VG_INFO("[Undo/Redo] Begin Redo Group \"%s\" (0x%016X)", GetEntryName().c_str(), this);

        for (auto * entry : m_subEntries)
            entry->Redo();

        VG_INFO("[Undo/Redo] End Redo Group \"%s\" (0x%016X)", GetEntryName().c_str(), this);
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoEntryGroup::GetEntryName() const
    {
        return m_entryName;
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoEntryGroup::GetObjectName() const
    {
        return fmt::sprintf("%u Object%s", m_subEntries.size(), m_subEntries.size() > 1 ? "s" : "");
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoEntryGroup::GetDescription() const
    {
        return "";
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoEntryGroup::AddSubEntry(IUndoRedoEntry * _subEntry)
    {
        m_subEntries.push_back(_subEntry);
    }

    //--------------------------------------------------------------------------------------
    const vector<IUndoRedoEntry *> * UndoRedoEntryGroup::GetSubEntries() const
    {
        return &m_subEntries;
    }
}