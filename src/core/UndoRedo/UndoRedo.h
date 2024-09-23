#pragma once

#include "core/IUndoRedo.h"
#include "core/File/Buffer.h"

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

    //--------------------------------------------------------------------------------------
    // TODO: do not store object raw pointers but safe GUID instead
    //--------------------------------------------------------------------------------------
    class UndoRedoPropertyEntry final : public UndoRedoEntry
    {
    public:
        UndoRedoPropertyEntry(IObject * _object, const IProperty * _prop, IObject * _originalObject, IGameObject * _prefab, IDynamicProperty * _propOverride);

        void BeforeChange() final override;
        void AfterChange() final override;

        void Undo() final override;
        void Redo() final override;

    private:
        io::Buffer m_original;
        io::Buffer m_modified;

        IObject * m_object;
        IProperty * m_prop;

        IObject * m_originalObject;
        IGameObject * m_prefab;
        IDynamicProperty * m_propOverride;
    };

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
        void AfterChange(IUndoRedoEntry * _entry) final override;

    protected:
        void destroyEntry(UndoRedoEntry * _entry);

    private:
        core::vector<UndoRedoEntry *>   m_doneStack;
        core::vector<UndoRedoEntry *>   m_undoneStack;
    };
}