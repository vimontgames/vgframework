#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    using NodeIndex = core::i16;

    class ISkeleton : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISkeleton, core::Object)

        virtual core::uint              GetNodeCount            () const = 0;
        virtual const core::string      GetNodeName             (NodeIndex _index) const = 0;
        virtual const core::float4x4 &  GetNodeWorldMatrix      (NodeIndex _index) const = 0;
        virtual NodeIndex               GetParentIndex          (NodeIndex _index) const = 0;
        virtual void                    SelectNode              (NodeIndex _index, bool _selected) = 0;
        virtual bool                    IsNodeSelected          (NodeIndex _index) const = 0;
        virtual NodeIndex               FindNodeIndex           (const core::string & _name) const = 0;
    };
}