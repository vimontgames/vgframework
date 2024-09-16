#include "SoundResource.h"
#include "audio/IAudio.h"
#include "audio/ISound.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(SoundResource, "Sound");

    //--------------------------------------------------------------------------------------
    bool SoundResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumBitfield(SoundResource, audio::SoundFlags, m_soundSettings.m_flags, "Flags");
        setPropertyDescription(SoundResource, m_soundSettings.m_flags, "Sound flags")

        registerProperty(SoundResource, m_soundSettings.m_volume, "Volume");
        setPropertyRange(SoundResource, m_soundSettings.m_volume, float2(0, 1));
        setPropertyDescription(SoundResource, m_soundSettings.m_volume, "Sound volume")

        registerPropertyCallbackEx(SoundResource, playSound, editor::style::icon::Play, PropertyFlags::SingleLine);
        setPropertyDescription(SoundResource, playSound, "Play sound");

        registerPropertyCallbackEx(SoundResource, stopSound, editor::style::icon::Stop, PropertyFlags::SingleLine);
        setPropertyDescription(SoundResource, stopSound, "Stop sound");

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::playSound(IObject * _object)
    {
        return ((SoundResource *)_object)->play();
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::stopSound(IObject * _object)
    {
        return ((SoundResource *)_object)->stop();
    }

    //--------------------------------------------------------------------------------------
    SoundResource::SoundResource(const string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    SoundResource::~SoundResource()
    {

    }

    //--------------------------------------------------------------------------------------
    const vector<string> SoundResource::GetExtensions() const
    {
        vector<string> ext;
        ext.push_back(".wav");
        ext.push_back(".ogg");
        ext.push_back(".mp3");
        ext.push_back(".flac");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    IObject * SoundResource::Load(const string & _file)
    {
        return Engine::get()->GetAudio()->CreateSound(_file); 
    }

    //--------------------------------------------------------------------------------------
    IResourceMeta * SoundResource::CreateResourceMeta(const core::string & _path) const
    {
        return nullptr; // new SoundResourceMeta(_path);
    }

    //--------------------------------------------------------------------------------------
    audio::PlaySoundHandle SoundResource::play()
    {
        if (m_playSoundHandle)
        {
            stop();
            m_playSoundHandle = 0;
        }

        if (auto * sound = getSound())
            m_playSoundHandle = Engine::get()->GetAudio()->Play(sound, m_soundSettings);

        return m_playSoundHandle;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::stop()
    {
        if (m_playSoundHandle)
        {
            if (Engine::get()->GetAudio()->Stop(m_playSoundHandle))
            {
                m_playSoundHandle = 0;
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::setVolume(const audio::PlaySoundHandle & _handle, float _volume)
    {
        if (_handle)
        {
            if (Engine::get()->GetAudio()->SetVolume(_handle, _volume))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::setLooping(const audio::PlaySoundHandle & _handle, bool _looping) 
    {
        if (_handle)
        {
            if (Engine::get()->GetAudio()->SetLooping(_handle, _looping))
                return true;
        }

        return false;
    }
}