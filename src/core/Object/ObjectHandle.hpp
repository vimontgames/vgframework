#include "ObjectHandle.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    ObjectHandle::ObjectHandle(core::UID _uid)
    {
        setUID(_uid);
    }

    //--------------------------------------------------------------------------------------
    ObjectHandle::ObjectHandle(const ObjectHandle & _other)
    {
        setUID(_other.getUID());
    }

    //--------------------------------------------------------------------------------------
    void ObjectHandle::setUID(core::UID _uid)
    {
        m_uid = _uid;
    }

    //--------------------------------------------------------------------------------------
    UID ObjectHandle::getUID() const
    {
        return m_uid;
    }

    //--------------------------------------------------------------------------------------
    const UID * ObjectHandle::getUIDPtr() const
    {
        return &m_uid;
    }

    //--------------------------------------------------------------------------------------
    void ObjectHandle::set(core::IObject * _object)
    {
        if (_object)
        {
            VG_ASSERT(_object->GetUID(false));
            setUID(_object->GetUID());
        }
        else
        {
            clear();
        }
    }

    //--------------------------------------------------------------------------------------
    IObject * ObjectHandle::getObject() const
    {
        auto * factory = Kernel::getFactory();
        return factory->FindByUID(m_uid);
    }

    //--------------------------------------------------------------------------------------
    void ObjectHandle::clear()
    {
        setUID(0x0);
    }
}
