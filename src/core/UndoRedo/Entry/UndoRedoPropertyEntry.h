#pragma once

#include "core/IUndoRedo.h"
#include "core/File/Buffer.h"

namespace vg::core
{
    class IObject;
    class IProperty;
    class IDynamicProperty;

    //--------------------------------------------------------------------------------------
    // TODO: do not store object raw pointers but safe GUID instead
    //--------------------------------------------------------------------------------------
    class UndoRedoPropertyEntry final : public UndoRedoEntry
    {
        using super = UndoRedoEntry;

    public:
        UndoRedoPropertyEntry(IObject * _object, const IProperty * _prop, IObject * _originalObject = nullptr, IGameObject * _prefab = nullptr, IDynamicProperty * _propOverride = nullptr);

        void BeforeChange() final override;
        void AfterChange() final override;

        void Undo() final override;
        void Redo() final override;

        string GetEntryName() const final override;
        string GetObjectName() const final override;
        string GetDescription() const final override;

    private:
        io::Buffer m_original;
        io::Buffer m_modified;

        IProperty * m_prop;
        IGameObject * m_prefab;
        IDynamicProperty * m_propOverride;
    };
}