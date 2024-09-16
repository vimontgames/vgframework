#pragma once

#include "core/Component/Component.h"

namespace vg::audio
{
    using PlaySoundHandle = core::u64;
}

namespace vg::engine
{
    class ISoundComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISoundComponent, core::Component);

        //virtual audio::PlaySoundHandle  Play() = 0;
        //virtual bool                    Stop() = 0;
    };
}