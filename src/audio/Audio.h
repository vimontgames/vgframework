#pragma once

#include "audio/IAudio.h"
#include "core/Singleton/Singleton.h"

namespace vg::audio
{
	class Audio final : public IAudio, public core::Singleton<Audio>
	{
	public:
        using super = IAudio;

                                            Audio		                (const core::string & _name, core::IObject * _parent);
                                            ~Audio                      ();

		IPlugin::Version				    GetVersion	                () const final override;
        const char *						GetClassName		        () const final override { return "Audio"; }

        bool                                RegisterClasses		        () final override;
        bool                                UnregisterClasses	        () final override;

        static bool                         registerProperties	        (core::IClassDesc & _desc);

        void							    Init                        (const AudioCreationParams & _params, core::Singletons & _singletons) final override;
        void							    Deinit                      () final override;
        void                                Update                      (float _dt) final override;
        IAudioOptions *                     GetOptions                  () const final override;
        ISound *                            CreateSound                 (const core::string & _path) final override;
        PlaySoundHandle                     Play                        (const ISound * _sound, const SoundSettings & _settings) final override;
        bool                                Stop                        (const PlaySoundHandle & _handle) final override;
        bool                                SetVolume                   (const PlaySoundHandle & _handle, float _volume) final override;
        bool                                SetLooping                  (const PlaySoundHandle & _handle, bool _looping) final override;

        VG_INLINE SoLoud::Soloud &          getSLEngine                 ();
        
    private:
        AudioCreationParams                 m_audioCreationParams;
        SoLoud::Soloud                      m_soloud;
	};
}

#if VG_ENABLE_INLINE
#include "Audio.inl"
#endif