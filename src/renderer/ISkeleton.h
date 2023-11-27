#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    class ISkeleton : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISkeleton, core::Object)

        virtual core::uint              GetNodeCount    () const = 0;
        virtual const core::string      GetNodeName     (core::uint _index) const = 0;
        virtual core::i16               GetParentIndex  (core::uint _index) const = 0;
    };
}