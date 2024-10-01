#include "UndoRedoDestroyEntry.h"
#include "core/Kernel.h"
#include "core/Object/Factory.h"
#include "core/IGameObject.h"
#include "core/string/string.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoDestroyEntry::UndoRedoDestroyEntry(IObject * _object, IObject * _parent, uint _indexInParent) :
        UndoRedoEntry(_object)
    {
        m_className = _object->GetClassName();
        m_parentUID = _parent->GetUID();
        m_indexInParent = _indexInParent;
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoDestroyEntry::BeforeChange()
    {
        Factory * factory = (Factory *)Kernel::getFactory();
        if (auto * object = GetObject())
        {
            m_buffer.resetWrite();
            factory->serializeObjectToMemory(object, m_buffer, BufferType::UndoRedo);
        }
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoDestroyEntry::AfterChange()
    {
             
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoDestroyEntry::GetEntryName() const
    {
        return "Destroy";
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoDestroyEntry::GetObjectName() const
    {
        return super::GetObjectName();
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoDestroyEntry::GetDescription() const
    {
        return fmt::sprintf("(%s)", m_className);
    }

    //--------------------------------------------------------------------------------------
    IObject * UndoRedoDestroyEntry::getParent() const
    {
        IFactory * factory = Kernel::getFactory();
        return factory->FindByUID(m_parentUID);
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoDestroyEntry::Undo()
    {
        VG_INFO("[Undo/Redo] Undo Destroy Object %s (UID=0x%08X)", GetDescription().c_str(), m_objectUID);

        Factory * factory = (Factory *)Kernel::getFactory();

        if (IGameObject * parent = dynamic_cast<IGameObject*>(getParent()))
        {
            IGameObject * obj = (IGameObject*)factory->CreateObject(m_className.c_str(), m_objectName, parent);
            m_buffer.resetRead();
            factory->serializeObjectFromMemory(obj, m_buffer, BufferType::UndoRedo);
            VG_ASSERT(m_objectUID == obj->GetUID());

            parent->AddChild(obj);
            obj->OnLoad();
            obj->Release();
        }
    }

    //--------------------------------------------------------------------------------------
    void UndoRedoDestroyEntry::Redo()
    {
        VG_INFO("[Undo/Redo] Redo Destroy Object \"%s\" (UID=0x%08X)", m_objectName.c_str(), m_objectUID);

        if (auto * obj = dynamic_cast<IGameObject *>(GetObject()))
        {
            if (auto * parent = dynamic_cast<IGameObject *>(obj->GetParent()))
                parent->RemoveChild(obj);
        }
    }
}