#pragma once
#include "audio/ISoundInstance.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class SoundInstance final : public ISoundInstance
    {
    public:
        VG_CLASS_DECL(SoundInstance, ISoundInstance);

        SoundInstance(const core::string & _name, core::IObject * _parent);
        ~SoundInstance();

        bool TryGetAABB(core::AABB & _aabb) const;

    private:
        AudioSourceType         m_audioSourceType = AudioSourceType::None;
        SoLoud::AudioSource *   m_slAudioSource = nullptr;
    };
}