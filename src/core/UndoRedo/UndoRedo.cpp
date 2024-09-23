#include "core/Precomp.h"
#include "UndoRedo.h"
#include "core/Kernel.h"
#include "core/Object/Factory.h"
#include "core/IGameObject.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoPropertyEntry::UndoRedoPropertyEntry(IObject * _object, const IProperty * _prop, IObject * _originalObject, IGameObject * _prefab, IDynamicProperty * _propOverride)
    {
        m_object = _object;
        m_prop = (IProperty*)_prop;
        
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

    //--------------------------------------------------------------------------------------
    UndoRedoManager::UndoRedoManager()
    {

    }

    //--------------------------------------------------------------------------------------
    UndoRedoManager::~UndoRedoManager()
    {
        Clear();
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::Undo()
    {
        if (HasUndo())
        {
            //VG_INFO("[Undo/Redo] Undo");

            if (m_doneStack.empty())
                return false;

            UndoRedoEntry * entry = m_doneStack.back();
            m_doneStack.pop_back();

            entry->Undo();
            m_undoneStack.push_back(entry);
            return true;
        }

        VG_INFO("[Undo/Redo] Nothing to Undo");
        return false;
    }
    
    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::Redo()
    {
        if (HasRedo())
        {
            //VG_INFO("[Undo/Redo] Redo");

            UndoRedoEntry * entry = m_undoneStack.back();
            m_undoneStack.pop_back();

            entry->Redo();
            m_doneStack.push_back(entry);

            return true;
        }

        VG_INFO("[Undo/Redo] Nothing to Redo");
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::HasUndo() const
    {
        return !m_doneStack.empty();
    }
    
    //--------------------------------------------------------------------------------------
    bool UndoRedoManager::HasRedo() const
    {
        return !m_undoneStack.empty();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::Clear()
    {
        for (UndoRedoEntry * cmd : m_doneStack)
            destroyEntry(cmd);
        m_doneStack.clear();

        for (UndoRedoEntry * cmd : m_undoneStack)
            destroyEntry(cmd);
        m_undoneStack.clear();
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::BeforeChange(IUndoRedoEntry * _entry)
    {
        VG_ASSERT(nullptr != _entry);
        if (_entry)
        {
            _entry->BeforeChange();
        }
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::AfterChange(IUndoRedoEntry * _entry)
    {
        VG_ASSERT(nullptr != _entry);
        if (_entry)
        {
            _entry->AfterChange();
            m_doneStack.push_back((UndoRedoEntry *)_entry);
            for (UndoRedoEntry * entry : m_undoneStack)
                destroyEntry(entry);
            m_undoneStack.clear();
        }
    }

    //--------------------------------------------------------------------------------------
    //void UndoRedoManager::createEntry(UndoRedoEntry * _entry)
    //{
    //    //return ma
    //}

    //--------------------------------------------------------------------------------------
    void UndoRedoManager::destroyEntry(UndoRedoEntry * _entry)
    {
        if (_entry)
            delete _entry;
    }
}