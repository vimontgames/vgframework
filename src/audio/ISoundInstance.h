#pragma once
#include "core/Instance/Instance.h"

namespace vg::audio
{
    class ISoundInstance : public core::Instance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISoundInstance, core::Instance);
    };
}