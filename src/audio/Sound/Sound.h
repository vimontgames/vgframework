#pragma once
#include "audio/ISound.h"

namespace vg::audio
{
    class Sound final : public ISound
    {
    public:
        VG_CLASS_DECL(Sound, ISound);

        Sound(const core::string & _name, core::IObject * _parent);
        ~Sound();

        //void SetSoundFlags(SoundFlags _flags, bool _enabled) final override;
        //SoundFlags GetSoundFlags() const final override;

        SoLoud::AudioSource * getSLAudioSource() const { return m_slAudioSource; }

    protected:
        static AudioSourceType getAudioSourceTypeFromFilename(const core::string & _filename);

    private:
        //SoundFlags              m_soundFlags = (SoundFlags)0x0;
        AudioSourceType         m_audioSourceType = AudioSourceType::None;
        SoLoud::AudioSource *   m_slAudioSource = nullptr;
    };
}