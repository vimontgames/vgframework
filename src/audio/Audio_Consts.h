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
}