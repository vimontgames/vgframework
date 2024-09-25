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
    public:
        UndoRedoPropertyEntry(IObject * _object, const IProperty * _prop, IObject * _originalObject, IGameObject * _prefab, IDynamicProperty * _propOverride);

        void BeforeChange() final override;
        void AfterChange() final override;

        void Undo() final override;
        void Redo() final override;

        string GetName() const final override;
        string GetDescription() const final override;

    private:
        io::Buffer m_original;
        io::Buffer m_modified;

        IObject * m_object;
        IProperty * m_prop;

        IObject * m_originalObject;
        IGameObject * m_prefab;
        IDynamicProperty * m_propOverride;
    };
}