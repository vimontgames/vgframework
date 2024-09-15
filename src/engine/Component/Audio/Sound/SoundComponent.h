#pragma once

#include "engine/ISoundComponent.h"
#include "engine/Resource/Audio/SoundResource.h"

namespace vg::engine
{
    class SoundComponent final : public ISoundComponent
    {
    public:
        VG_CLASS_DECL(SoundComponent, ISoundComponent);

        SoundComponent(const core::string & _name, IObject * _parent);
        ~SoundComponent();

        void                    OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent);

        void                    OnPlay() final override;
        void                    OnStop() final override;

        audio::PlaySoundHandle  Play() final override;
        bool                    Stop() final override;

    protected:
        static bool             playSound(IObject * _object);
        static bool             stopSound(IObject * _object);

    private:
        SoundResource           m_soundRes;
        audio::SoundSettings    m_soundSettings;
        audio::PlaySoundHandle  m_playSoundHandle = 0;
    };
}