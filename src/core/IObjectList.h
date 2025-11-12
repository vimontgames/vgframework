#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IObjectList : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IObjectList, Object)

        virtual bool    Add         () = 0;
        virtual bool    Pop         () = 0;
        virtual bool    RemoveAt    (size_t _index) = 0;
        virtual size_t  Size        () const = 0;
    };
}