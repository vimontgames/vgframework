#pragma once

#include "core/IUndoRedo.h"
#include "core/File/Buffer.h"

namespace vg::core
{
    class IObject;
    class IProperty;
    class IDynamicProperty;

    //--------------------------------------------------------------------------------------
    class UndoRedoDestroyEntry final : public UndoRedoEntry
    {
        using super = UndoRedoEntry;

    public:
        UndoRedoDestroyEntry(IObject * _object, IObject * _parent, uint _indexInParent);

        void BeforeChange() final override;
        void AfterChange() final override;

        void Undo() final override;
        void Redo() final override;

        string GetEntryName() const final override;
        string GetObjectName() const final override;
        string GetDescription() const final override;

        IObject * getParent() const;

    private:
        io::Buffer m_buffer;
        string m_className;
        UID m_parentUID;
        uint m_indexInParent;
    };
}