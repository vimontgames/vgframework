#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class IResourceList : public core::Object
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(IResourceList, core::Object)

        virtual bool Add() = 0;
        virtual bool Remove() = 0;
    };
}