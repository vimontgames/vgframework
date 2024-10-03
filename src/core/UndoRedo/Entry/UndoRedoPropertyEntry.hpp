#include "UndoRedoPropertyEntry.h"
#include "core/Kernel.h"
#include "core/Object/Factory.h"
#include "core/IGameObject.h"
#include "core/string/string.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoPropertyEntry::UndoRedoPropertyEntry(IObject * _object, const IProperty * _prop, IObject * _originalObject, IGameObject * _prefab, IDynamicProperty * _propOverride) :
        UndoRedoEntry(_object)
    {
        if (nullptr != _originalObject)
            m_objectName = _originalObject->GetName();

        VG_ASSERT(_prop);
        m_prop = *((Property *)_prop);
        m_prefab = _prefab;
        m_propOverride = _propOverride;
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::BeforeChange()
    {
        Factory * factory = (Factory *)Kernel::getFactory();
        if (auto * obj = GetObject())
            factory->serializePropertyToMemory(obj, &m_prop, m_original, BufferType::UndoRedo);
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::AfterChange()
    {
        Factory * factory = (Factory *)Kernel::getFactory();
        if (auto * obj = GetObject())
            factory->serializePropertyToMemory(obj, &m_prop, m_modified, BufferType::UndoRedo);
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoPropertyEntry::GetEntryName() const
    {
        return "Property";
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoPropertyEntry::GetObjectName() const
    {
        return super::GetObjectName();
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoPropertyEntry::GetDescription() const
    {
        return fmt::sprintf("(%s)%s", asString(m_prop.GetType()), m_prop.GetDisplayName());
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::Undo()
    {
        if (auto * object = GetObject())
        {
            // Restore the original value serialized in memory before change
            VG_INFO("[Undo/Redo] Undo Property \"%s\" (0x%016X) from Object \"%s\" (0x%016X)", m_prop.GetName(), m_prop, m_objectName.c_str(), object);

            m_original.resetRead();
            Factory * factory = (Factory *)Kernel::getFactory();
            factory->serializePropertyFromMemory(object, &m_prop, m_original, BufferType::UndoRedo);

            if (m_prefab)
            {
                auto & children = m_prefab->GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    m_prefab->OverrideGameObjectProperties(children[i], m_propOverride);
            }
            else
            {
                object->OnPropertyChanged(object, m_prop);
            }
        }
        else
        {
            VG_ERROR("[Undo/Redo] Cannot Undo Property \"%s\" (0x%016X) from Object \"%s\" (UID=0x%08X)", m_prop.GetName(), m_prop, m_objectName.c_str(), m_objectUID);
        }
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::Redo()
    {
        if (auto * object = GetObject())
        {
            // Restore the modified value serialized in memory before change
            VG_INFO("[Undo/Redo] Redo Property \"%s\" (0x%016X) from Object \"%s\" (0x%016X)", m_prop.GetName(), m_prop, m_objectName.c_str(), object);

            m_modified.resetRead();
            Factory * factory = (Factory *)Kernel::getFactory();
            factory->serializePropertyFromMemory(object, &m_prop, m_modified, BufferType::UndoRedo);

            if (m_prefab)
            {
                auto & children = m_prefab->GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    m_prefab->OverrideGameObjectProperties(children[i], m_propOverride);
            }
            else
            {
                object->OnPropertyChanged(object, m_prop);
            }
        }
        else
        {
            VG_ERROR("[Undo/Redo] Cannot Redo Property \"%s\" (0x%016X) from Object \"%s\" (UID=0x%08X)", m_prop.GetName(), &m_prop, m_objectName.c_str(), m_objectUID);
        }
    }
}