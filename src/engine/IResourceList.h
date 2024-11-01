#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class IResourceList : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IResourceList, core::Object)

        virtual bool    Add         () = 0;
        virtual bool    Pop         () = 0;
        virtual bool    RemoveAt    (core::size_t _index) = 0;
        virtual size_t  Size        () const = 0;
    };
}