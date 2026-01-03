#pragma once
#include "core/Options/Options.h"

namespace vg
{
    class IGameOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IGameOptions, core::Options);
    };
}