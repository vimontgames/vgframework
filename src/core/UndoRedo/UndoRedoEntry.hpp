#include "UndoRedoEntry.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    UndoRedoEntry::UndoRedoEntry(IObject * _object)
    {
        if (nullptr != _object)
        {
            m_objectUID = _object->GetUID(false);
            VG_ASSERT(m_objectUID, "(%s)\"%s\" has no UID", _object->GetClassName(), _object->GetShortName().c_str());

            m_objectName = _object->GetName();
        }
    }

    //--------------------------------------------------------------------------------------
    UndoRedoEntry::~UndoRedoEntry()
    {

    }

    //--------------------------------------------------------------------------------------
    IObject * UndoRedoEntry::GetObject() const
    {
        IFactory * factory = Kernel::getFactory();
        return factory->FindByUID(m_objectUID);
    }

    //--------------------------------------------------------------------------------------
    string UndoRedoEntry::GetObjectName() const
    {
        return m_objectName;
    }
}