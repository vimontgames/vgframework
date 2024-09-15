#pragma once
#include "core/Resource/Resource.h"
#include "audio/Audio_Consts.h"

namespace vg::audio
{
    class ISound;
}

namespace vg::engine
{
    class SoundResource : public core::Resource
    {
    public:
        VG_CLASS_DECL(SoundResource, core::Resource);

        SoundResource(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~SoundResource();

        const core::vector<core::string>    GetExtensions   () const final;
        core::IObject *                     Load            (const core::string & _file) final override;

        bool                                play            ();
        bool                                stop            ();

    protected:
        static bool                         playSound       (IObject * _object);
        static bool                         stopSound       (IObject * _object);

        audio::ISound *                     getSound        () const { return (audio::ISound *)GetObject(); }
        audio::PlaySoundHandle              getSoundHandle  () const { return m_playSoundHandle; }
        void                                setSoundHandle  (audio::PlaySoundHandle _handle) { m_playSoundHandle = _handle; }

    private:
        audio::PlaySoundHandle              m_playSoundHandle = 0;
    };
}