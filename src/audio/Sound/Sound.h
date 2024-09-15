#pragma once
#include "audio/ISound.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class Sound final : public ISound
    {
    public:
        VG_CLASS_DECL(Sound, ISound);

        Sound(const core::string & _name, core::IObject * _parent);
        ~Sound();

        SoLoud::AudioSource * getSLAudioSource() const { return m_slAudioSource; }

    protected:
        static AudioSourceType getAudioSourceTypeFromFilename(const core::string & _filename);

    private:
        AudioSourceType         m_audioSourceType = AudioSourceType::None;
        SoLoud::AudioSource *   m_slAudioSource = nullptr;
    };
}