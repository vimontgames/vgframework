#include "UndoRedoPropertyEntry.h"
#include "core/Kernel.h"
#include "core/Object/Factory.h"
#include "core/IGameObject.h"
#include "core/string/string.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoPropertyEntry::UndoRedoPropertyEntry(IObject * _object, const IProperty * _prop, IObject * _originalObject, IGameObject * _prefab, IDynamicProperty * _propOverride)
    {
        m_object = _object;
        m_prop = (IProperty *)_prop;

        m_originalObject = _originalObject;
        m_prefab = _prefab;
        m_propOverride = _propOverride;
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::BeforeChange()
    {
        Factory * factory = (Factory *)Kernel::getFactory();
        factory->serializePropertyToMemory(m_object, m_prop, m_original);
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::AfterChange()
    {
        Factory * factory = (Factory *)Kernel::getFactory();
        factory->serializePropertyToMemory(m_object, m_prop, m_modified);
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoPropertyEntry::GetName() const
    {
        return "Edit property";
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoPropertyEntry::GetDescription() const
    {
        return fmt::sprintf("(%s)%s", asString(m_prop->GetType()), m_prop->GetName());
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::Undo()
    {
        // Restore the original value serialized in memory before change
        VG_INFO("[Undo/Redo] Undo Property \"%s\" (0x%016X) from Object \"%s\" (0x%016X)", m_prop->GetName(), m_prop, m_originalObject->getName().c_str(), m_object);

        m_original.resetRead();
        Factory * factory = (Factory *)Kernel::getFactory();
        factory->serializePropertyFromMemory(m_object, m_prop, m_original);

        if (m_prefab)
        {
            auto & children = m_prefab->GetChildren();
            for (uint i = 0; i < children.size(); ++i)
                m_prefab->OverrideGameObjectProperties(children[i], m_propOverride);
        }
        else
        {
            m_object->OnPropertyChanged(m_object, *m_prop);
        }
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoPropertyEntry::Redo()
    {
        // Restore the modified value serialized in memory before change
        VG_INFO("[Undo/Redo] Redo Property \"%s\" (0x%016X) from Object \"%s\" (0x%016X)", m_prop->GetName(), m_prop, m_originalObject->getName().c_str(), m_object);

        m_modified.resetRead();
        Factory * factory = (Factory *)Kernel::getFactory();
        factory->serializePropertyFromMemory(m_object, m_prop, m_modified);

        if (m_prefab)
        {
            auto & children = m_prefab->GetChildren();
            for (uint i = 0; i < children.size(); ++i)
                m_prefab->OverrideGameObjectProperties(children[i], m_propOverride);
        }
        else
        {
            m_object->OnPropertyChanged(m_object, *m_prop);
        }
    }
}