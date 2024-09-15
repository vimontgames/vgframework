#pragma once

#include "engine/ISoundComponent.h"
#include "engine/Resource/Audio/SoundResource.h"

namespace vg::engine
{
    class SoundComponent final : public ISoundComponent
    {
    public:
        VG_CLASS_DECL(SoundComponent, ISoundComponent);

        SoundComponent(const core::string & _name, IObject * _parent);
        ~SoundComponent();

        audio::PlaySoundHandle Play() final override;

    private:
        SoundResource   m_soundRes;
    };
}