#pragma once
#include "engine/ISoundResource.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class ISound;
}

namespace vg::engine
{
    class SoundResource final : public ISoundResource
    {
    public:
        VG_CLASS_DECL(SoundResource, ISoundResource);

        SoundResource(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~SoundResource();

        const core::vector<core::string>    GetExtensions       () const final;
        core::LoadStatus                    Load                (const core::string & _file, core::IObject *& _object) final override;
        core::IResourceMeta *               CreateResourceMeta  (const core::string & _path) const final override;
        void                                OnPropertyChanged   (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);

        void                                OnPlay              () final override;
        void                                OnStop              () final override;

        audio::PlaySoundHandle              play                ();
        bool                                stop                ();
        bool                                setVolume           (const audio::PlaySoundHandle & _handle, float _volume);
        bool                                setLooping          (const audio::PlaySoundHandle & _handle, bool _looping);

    private:
        static bool                         playSound           (IObject * _object);
        static bool                         stopSound           (IObject * _object);

        audio::ISound *                     getSound            () const { return (audio::ISound *)GetObject(); }

    private:
        audio::SoundSettings                m_soundSettings;
        audio::PlaySoundHandle              m_playSoundHandle = 0;
    };
}