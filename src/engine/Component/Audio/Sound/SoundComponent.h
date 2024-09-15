#pragma once

#include "engine/ISoundComponent.h"

namespace audio
{
    class ISoundInstance;
}

namespace vg::engine
{
    class SoundComponent final : public ISoundComponent
    {
    public:
        VG_CLASS_DECL(SoundComponent, ISoundComponent);

        SoundComponent(const core::string & _name, IObject * _parent);
        ~SoundComponent();

    private:
        core::string            m_path;
        audio::ISoundInstance * m_soundInstance = nullptr;
    };
}