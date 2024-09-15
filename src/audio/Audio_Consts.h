#pragma once

namespace vg::audio
{
    struct AudioCreationParams
    {
        
    };

    vg_enum_class(AudioSourceType, core::u8,
        None,
        Wav,
        Ogg,
        Mp3,
        Flac
    );

    vg_enum_class(SoundFlags, core::u32,
        PlayOnStart = 0x00000001,
        Loop        = 0x00000002,
        Background  = 0x80000000 
    );

    struct SoundSettings
    {
        SoundFlags  m_flags = (SoundFlags)0x0;
        float       m_volume = 1.0f;
    };

    using PlaySoundHandle = core::u64;
}