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

        registerPropertyCallbackEx(SoundResource, playSound, "Play", PropertyFlags::SingleLine);
        registerPropertyCallbackEx(SoundResource, stopSound, "Stop", PropertyFlags::SingleLine);

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
    bool SoundResource::play()
    {
        if (auto * sound = getSound())
        {
            if (auto handle = Engine::get()->GetAudio()->PlaySound(sound))
            {
                setSoundHandle(handle);
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::stop()
    {
        if (auto handle = getSoundHandle())
        {
            if (Engine::get()->GetAudio()->StopSound(handle))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::playSound(IObject * _object)
    {
        return ((SoundResource*)_object)->play();
    }

    //--------------------------------------------------------------------------------------
    bool SoundResource::stopSound(IObject * _object)
    {
        return ((SoundResource *)_object)->stop();
    }
}