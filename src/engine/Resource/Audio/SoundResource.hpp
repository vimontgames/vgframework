#include "SoundResource.h"
#include "audio/IAudio.h"
#include "audio/ISound.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(SoundResource, "Sound");

    //--------------------------------------------------------------------------------------
    bool SoundResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
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
    audio::PlaySoundHandle SoundResource::play(const audio::SoundSettings & _settings)
    {
        if (auto * sound = getSound())
        {
            if (auto handle = Engine::get()->GetAudio()->Play(sound, _settings))
                return handle;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::stop(const audio::PlaySoundHandle & _handle)
    {
        if (_handle)
        {
            if (Engine::get()->GetAudio()->Stop(_handle))
                return true;
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