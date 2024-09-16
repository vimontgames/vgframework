#pragma once

#include "engine/ISoundComponent.h"
#include "engine/Component/Audio/Sound/SoundResourceList.h"

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

        //audio::PlaySoundHandle  Play() final override;
        //bool                    Stop() final override;

    private:
        SoundResourceList       m_sounds;
    };
}