#pragma once
#include "core/Resource/Resource.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class ISound;
}

namespace vg::engine
{
    class SoundResource final : public core::Resource
    {
    public:
        VG_CLASS_DECL(SoundResource, core::Resource);

        SoundResource(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~SoundResource();

        const core::vector<core::string>    GetExtensions       () const final;
        core::IObject *                     Load                (const core::string & _file) final override;
        core::IResourceMeta *               CreateResourceMeta  (const core::string & _path) const final override;

        audio::PlaySoundHandle              play                (const audio::SoundSettings & _settings);
        bool                                stop                (const audio::PlaySoundHandle & _handle);
        bool                                setVolume           (const audio::PlaySoundHandle & _handle, float _volume);
        bool                                setLooping          (const audio::PlaySoundHandle & _handle, bool _looping);

    private:
        audio::ISound *                     getSound            () const { return (audio::ISound *)GetObject(); }
    };
}