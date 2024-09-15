#include "audio/Precomp.h"
#include "Sound.h"
#include "audio/Audio.h"
#include "core/string/string.h"
#include "core/File/File.h"

using namespace vg::core;

namespace vg::audio
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(Sound, "Sound instance");

    //--------------------------------------------------------------------------------------
    bool Sound::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    Sound::Sound(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        m_audioSourceType = getAudioSourceTypeFromFilename(_name);

        switch (m_audioSourceType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_audioSourceType);
                break;

            case AudioSourceType::Wav:
                m_slAudioSource = new SoLoud::Wav();
                VG_VERIFY(SoLoud::SO_NO_ERROR == ((SoLoud::Wav*)m_slAudioSource)->load(_name.c_str()));
                break;

            case AudioSourceType::Mp3:
                m_slAudioSource = new SoLoud::Wav();
                VG_VERIFY(SoLoud::SO_NO_ERROR == ((SoLoud::Wav *)m_slAudioSource)->load(_name.c_str()));
                break;
        }
    }

    //--------------------------------------------------------------------------------------
    Sound::~Sound()
    {
        VG_SAFE_DELETE(m_slAudioSource);
    }

    ////--------------------------------------------------------------------------------------
    //void Sound::SetSoundFlags(SoundFlags _flags, bool _enabled)
    //{
    //    if (_enabled)
    //        m_soundFlags |= _flags;
    //    else
    //        m_soundFlags &= ~_flags;
    //
    //    if (asBool(SoundFlags::Loop & _flags))
    //        m_slAudioSource->setLooping(_enabled);
    //}
    //
    ////--------------------------------------------------------------------------------------
    //SoundFlags Sound::GetSoundFlags() const
    //{
    //    return m_soundFlags;
    //}

    //--------------------------------------------------------------------------------------
    AudioSourceType Sound::getAudioSourceTypeFromFilename(const core::string & _filename)
    {
        const string ext = io::getFileExtension(_filename);

        if (ext == "wav")
            return AudioSourceType::Wav;
        else if (ext == "ogg")
            return AudioSourceType::Ogg;
        else if (ext == "mp3")
            return AudioSourceType::Mp3;
        else if (ext == "flac")
            return AudioSourceType::Flac;

        VG_WARNING("[Audio] Could not get AudioSourceType for file \"%s\"", _filename);
        return AudioSourceType::None;
    }
}