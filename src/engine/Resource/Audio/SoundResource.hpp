#include "SoundResource.h"
#include "audio/IAudio.h"
#include "audio/ISound.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(SoundResource, "Sound");

    //--------------------------------------------------------------------------------------
    void * ResizeSoundResourceVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<SoundResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(SoundResource, m_name, core::PropertyFlags::NotVisible, false);

        registerPropertyEnumBitfield(SoundResource, audio::SoundFlags, m_soundSettings.m_flags, "Flags");
        setPropertyDescription(SoundResource, m_soundSettings.m_flags, "Sound flags")

        registerProperty(SoundResource, m_soundSettings.m_volume, "Volume");
        setPropertyRange(SoundResource, m_soundSettings.m_volume, float2(0, 1));
        setPropertyDescription(SoundResource, m_soundSettings.m_volume, "Sound volume")

        registerProperty(SoundResource, m_soundSettings.m_delay, "Delay");
        setPropertyRange(SoundResource, m_soundSettings.m_delay, float2(0, 60.0f));
        setPropertyDescription(SoundResource, m_soundSettings.m_delay, "Sound delay")

        registerPropertyCallbackEx(SoundResource, playSound, editor::style::icon::Play, PropertyFlags::SingleLine);
        setPropertyDescription(SoundResource, playSound, "Play sound");

        registerPropertyCallbackEx(SoundResource, stopSound, editor::style::icon::Stop, PropertyFlags::SingleLine);
        setPropertyDescription(SoundResource, stopSound, "Stop sound");

        registerResizeVectorFunc(SoundResource, ResizeSoundResourceVector);

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
        super(_name, _parent)
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
    void SoundResource::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.GetName();
        if (!strcmp(name, "m_soundSettings.m_volume"))
        {
            if (m_playSoundHandle)
                setVolume(m_playSoundHandle, *_prop.GetPropertyFloat(_object));
        }
        else if(!strcmp(name, "m_soundSettings.m_flags"))
        {
            if (m_playSoundHandle)
            {
                audio::SoundFlags flags = *_prop.GetPropertyEnum<audio::SoundFlags>(_object);
                if (asBool(audio::SoundFlags::Loop & flags))
                    setLooping(m_playSoundHandle, true);
                else
                    setLooping(m_playSoundHandle, false);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void SoundResource::OnPlay()
    {
        super::OnPlay();
        if (asBool(audio::SoundFlags::PlayOnStart & m_soundSettings.m_flags))
            m_playSoundHandle = play();
    }

    //--------------------------------------------------------------------------------------
    void SoundResource::OnStop()
    {
        stop();
        super::OnStop();
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