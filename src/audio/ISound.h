#pragma once
#include "core/Model/Model.h"

namespace vg::audio
{
    class ISound : public core::Model
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISound, core::Model);
    };
}