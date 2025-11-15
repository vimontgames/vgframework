#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IResourceList : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IResourceList, Object)

        virtual bool    Add         (const string & _name) = 0;
        virtual bool    Pop         () = 0;
        virtual bool    RemoveAt    (size_t _index) = 0;
        virtual size_t  Size        () const = 0;
    };
}