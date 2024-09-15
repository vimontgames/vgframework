#include "audio/Precomp.h"
#include "audio.h"
#include "Options/AudioOptions.h"
#include "Sound/Sound.h"

#if !VG_ENABLE_INLINE
#include "audio.inl"
#endif

using namespace vg::core;
using namespace vg::audio;

#define VG_AUDIO_VERSION_MAJOR 0
#define VG_AUDIO_VERSION_MINOR 0

//--------------------------------------------------------------------------------------
IAudio * CreateNew()
{
	return new Audio("Audio", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::audio
{
	//--------------------------------------------------------------------------------------
	IPlugin::Version Audio::GetVersion() const
	{
		return { VG_AUDIO_VERSION_MAJOR, VG_AUDIO_VERSION_MINOR };
	}

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
        Timer::init();

        // Copy init params
        m_audioCreationParams = _params;

        // Set our singletons used by this DLL
        Kernel::setSingletons(_singletons);

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
    PlaySoundHandle Audio::PlaySound(const ISound * _sound)
    {
        if (_sound)
        {
            if (auto * slAudioSource = ((Sound *)_sound)->getSLAudioSource())
            {
                return (PlaySoundHandle)m_soloud.play(*slAudioSource);
            }
        }

        return PlaySoundHandle(0);
    }

    //--------------------------------------------------------------------------------------
    bool Audio::StopSound(const PlaySoundHandle & _handle)
    {
        if (0 != _handle)
        {
            m_soloud.stop((SoLoud::handle)_handle);
            return true;
        }

        return false;
    }
}