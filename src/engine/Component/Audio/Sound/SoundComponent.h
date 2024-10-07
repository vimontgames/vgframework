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

        void                    OnPlay          () final override;
        void                    OnStop          () final override;

        audio::PlaySoundHandle  Play            (core::uint _index) final override;
        bool                    Stop            (core::uint _index) final override;

        ISoundResource *        GetSound        (core::uint _index) const final override;
        core::uint              GetSoundIndex   (const core::string & _name) const final override;
        core::uint              GetSoundCount   () const final override;

    private:
        SoundResourceList       m_sounds;
    };
}