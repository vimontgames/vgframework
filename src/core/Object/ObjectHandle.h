#pragma once

#include "core/Types/Types.h"
#include "core/IFactory.h"

namespace vg::core
{
    class ObjectHandle
    {
    public:
        ObjectHandle(core::UID _uid = (UID)0);
        ObjectHandle(const ObjectHandle & _other);

        void        setUID      (core::UID _uid);
        UID         getUID      () const;
        const UID * getUIDPtr   () const;

        IObject *   getObject   () const;

        template <typename T> T* get() const
        {
            return dynamic_cast<T *>(getObject());
        }

    private:
        core::UID   m_uid = (UID)0;
    };
}