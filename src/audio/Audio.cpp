#include "audio/Precomp.h"
#include "audio.h"
#include "Options/AudioOptions.h"
#include "Sound/Sound.h"

#if !VG_ENABLE_INLINE
#include "audio.inl"
#endif

using namespace vg::core;
using namespace vg::audio;

//--------------------------------------------------------------------------------------
IAudio * CreateNew()
{
	return new Audio("Audio", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::audio
{
	//--------------------------------------------------------------------------------------
    Audio::Audio(const string & _name, IObject * _parent) :
        super(_name, _parent)
	{
        
	}

	//--------------------------------------------------------------------------------------
    Audio::~Audio()
	{

	}

    //--------------------------------------------------------------------------------------
    bool Audio::RegisterClasses()
    {
        IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register the "Audio" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (IClassDesc * desc = factory->registerPlugin(Audio, "Audio"))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Audio::UnregisterClasses()
    {
        return AutoRegisterClassInfo::unregisterClasses();
    }

    //--------------------------------------------------------------------------------------
    bool Audio::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

	//--------------------------------------------------------------------------------------
	void Audio::Init(const AudioCreationParams & _params, Singletons & _singletons)
	{
        // Set our singletons used by this DLL
        Kernel::setSingletons(_singletons);

        VG_PROFILE_CPU("Audio");

        Timer::init();

        // Copy init params
        m_audioCreationParams = _params;

        // Register all audio classes 
        RegisterClasses();
        
        AudioOptions * audioOptions = new AudioOptions("AudioOptions", this);

        // initialize SoLoud.
        m_soloud.init();
	}

	//--------------------------------------------------------------------------------------
	void Audio::Deinit()
	{
        AudioOptions * audioOptions = AudioOptions::get();
        VG_SAFE_DELETE(audioOptions);

        // Unregister our classes
        UnregisterClasses();

        // Shutdown sound engine
        m_soloud.deinit();
	}

	//--------------------------------------------------------------------------------------
	void Audio::Update(float _dt)
	{
        VG_PROFILE_CPU("Audio");
        GetOptions()->Update();
	}

    //--------------------------------------------------------------------------------------
    IAudioOptions * Audio::GetOptions() const
    {
        return AudioOptions::get();
    }  

    //--------------------------------------------------------------------------------------
    ISound * Audio::CreateSound(const core::string & _path)
    {
        return new Sound(_path, nullptr);
    }

    //--------------------------------------------------------------------------------------
    PlaySoundHandle Audio::Play(const ISound * _sound, const SoundSettings & _settings)
    {
        if (_sound)
        {
            if (auto * slAudioSource = ((Sound *)_sound)->getSLAudioSource())
            {
                PlaySoundHandle handle;
                
                if (asBool(SoundFlags::Background & _settings.m_flags))
                    handle = (PlaySoundHandle)m_soloud.playBackground(*slAudioSource, _settings.m_volume);
                else if (_settings.m_delay > 0.0f)
                    handle = (PlaySoundHandle)m_soloud.playClocked((double)_settings.m_delay, *slAudioSource, _settings.m_volume);
                else
                    handle = (PlaySoundHandle)m_soloud.play(*slAudioSource, _settings.m_volume);

                if (asBool(SoundFlags::Loop & _settings.m_flags))
                    m_soloud.setLooping((SoLoud::handle)handle, true);
                else
                    m_soloud.setLooping((SoLoud::handle)handle, false);

                return handle;
            }
        }

        return PlaySoundHandle(0);
    }

    //--------------------------------------------------------------------------------------
    bool Audio::Stop(const PlaySoundHandle & _handle)
    {
        if (0 != _handle)
        {
            m_soloud.stop((SoLoud::handle)_handle);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Audio::SetVolume(const PlaySoundHandle & _handle, float _volume)
    {
        if (0 != _handle)
        {
            m_soloud.setVolume((SoLoud::handle)_handle, _volume);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Audio::SetLooping(const PlaySoundHandle & _handle, bool _looping) 
    {
        if (0 != _handle)
        {
            m_soloud.setLooping((SoLoud::handle)_handle, _looping);
            return true;
        }

        return false;
    }
}