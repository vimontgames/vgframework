#pragma once
#include "core/Singleton/Singleton.h"
#include "audio/IAudioOptions.h"

namespace vg::audio
{
    class AudioOptions final : public IAudioOptions, public core::Singleton<AudioOptions>
    {
    public:
        VG_CLASS_DECL(AudioOptions, IAudioOptions);

        AudioOptions(const core::string & _name, core::IObject * _parent = nullptr);

        void                    OnPropertyChanged(core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) final override;
        void                    OnLoad() final override;

        static bool			    load(core::IObject * _object);
        static bool			    save(core::IObject * _object);

    private:

    };
}