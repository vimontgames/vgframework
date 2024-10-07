#pragma once
#include "core/Resource/Resource.h"

namespace vg::engine
{
    class ISoundResource : public core::Resource
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISoundResource, core::Resource);
    };
}