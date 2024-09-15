#pragma once
#include "core/Model/Model.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class ISound : public core::Model
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISound, core::Model);

        //virtual void SetSoundFlags(SoundFlags _flags, bool _enabled) = 0;
        //virtual SoundFlags GetSoundFlags() const = 0;
    };
}