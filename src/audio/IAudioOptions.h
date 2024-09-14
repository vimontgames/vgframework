#pragma once
#include "core/Options/Options.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class IAudioOptions : public core::Options
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IAudioOptions, core::Options);
    };
}