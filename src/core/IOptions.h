#pragma once
#include "core/Object/Object.h"

namespace vg::core
{
    class IOptions : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IOptions, core::Object);

        virtual void    Update() = 0;
        virtual bool    Load(bool _async) = 0;
        virtual bool    Save() = 0;
    };
}