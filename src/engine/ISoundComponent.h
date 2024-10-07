#pragma once

#include "core/Component/Component.h"

namespace vg::audio
{
    using PlaySoundHandle = core::u64;
}

namespace vg::engine
{
    class ISoundResource;

    class ISoundComponent : public core::Component
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ISoundComponent, core::Component);

        virtual audio::PlaySoundHandle  Play            (core::uint _index) = 0;
        virtual bool                    Stop            (core::uint _index) = 0;

        virtual ISoundResource *        GetSound        (core::uint _index) const = 0;
        virtual core::uint              GetSoundIndex   (const core::string & _name) const = 0;
        virtual core::uint              GetSoundCount   () const = 0;
    };
}