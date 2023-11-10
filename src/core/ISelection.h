#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class ISelection : public core::Object
    {
    public:
        using ObjectList = core::vector<core::IObject *>;

        virtual core::IObject * GetSelectedObject   () = 0;
        virtual void            SetSelectedObject   (core::IObject * _object) = 0;
        virtual ObjectList &    GetSelectedObjects  () = 0;
        virtual void            SetSelectedObjects  (const core::vector<core::IObject *> & _objects) = 0;
        virtual bool            IsSelectedObject    (core::IObject * _object) = 0;
        virtual bool            RemoveFromSelection (core::IObject * _object) = 0;
        virtual bool            AddToSelection      (core::IObject * _object) = 0;
    };
}