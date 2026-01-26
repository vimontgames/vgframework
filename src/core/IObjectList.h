#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IObjectList : public Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IObjectList, Object)

        virtual bool    Add         (const string & _name) = 0;
        virtual bool    Pop         () = 0;
        virtual bool    MoveUp      (size_t _index) = 0;
        virtual bool    MoveDown    (size_t _index) = 0;
        virtual bool    Remove      (size_t _index) = 0;
        virtual size_t  Size        () const = 0;

        virtual bool    CanAdd      () const = 0;
        virtual bool    CanRemove   () const = 0;

        virtual void    SetMinCount (size_t _count) = 0;
        virtual void    SetMaxCount (size_t _count) = 0;
    };
}