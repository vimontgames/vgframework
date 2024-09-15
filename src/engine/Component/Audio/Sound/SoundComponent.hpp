#include "SoundComponent.h"
#include "editor/Editor_Consts.h"
#include "core/Object/Update.h"

using namespace vg::core;
using namespace vg::audio;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(SoundComponent, "Sound", "Audio", "Audio sound component", editor::style::icon::Sound, 0);

    //--------------------------------------------------------------------------------------
    bool SoundComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyCallbackEx(SoundResource, playSound, editor::style::icon::Play, PropertyFlags::SingleLine);
        registerPropertyCallbackEx(SoundResource, stopSound, editor::style::icon::Stop, PropertyFlags::SingleLine);

        registerPropertyResource(SoundComponent, m_soundRes, "Path");
        registerPropertyEnumBitfield(SoundComponent, audio::SoundFlags, m_soundSettings.m_flags, "Flags");
        setPropertyDescription(SoundComponent, m_soundSettings.m_flags, "Sound flags")

        registerProperty(SoundComponent, m_soundSettings.m_volume, "Volume");
        setPropertyRange(SoundComponent, m_soundSettings.m_volume, float2(0, 1));
        setPropertyDescription(SoundComponent, m_soundSettings.m_volume, "Sound volume")

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool SoundComponent::playSound(IObject * _object)
    {
        return ((SoundComponent *)_object)->Play();
    }

    //--------------------------------------------------------------------------------------
    bool SoundComponent::stopSound(IObject * _object)
    {
        return ((SoundComponent *)_object)->Stop();
    }

    //--------------------------------------------------------------------------------------
    SoundComponent::SoundComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
        m_soundRes(_name, _parent)
    {
        SetUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update | UpdateFlags::LateUpdate, false);
    }

    //--------------------------------------------------------------------------------------
    SoundComponent::~SoundComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void SoundComponent::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.GetName();
        if (!strcmp(name, "m_soundSettings.m_volume"))
        {
            if (m_playSoundHandle)
                m_soundRes.setVolume(m_playSoundHandle, *_prop.GetPropertyFloat(_object));
        }
        else if(!strcmp(name, "m_soundSettings.m_flags"))
        {
            if (m_playSoundHandle)
            {
                audio::SoundFlags flags = *_prop.GetPropertyEnum<audio::SoundFlags>(_object);
                if (asBool(audio::SoundFlags::Loop & flags))
                    m_soundRes.setLooping(m_playSoundHandle, true);
                else
                    m_soundRes.setLooping(m_playSoundHandle, false);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void SoundComponent::OnPlay()
    {
        super::OnPlay();
        if (asBool(SoundFlags::PlayOnStart & m_soundSettings.m_flags))
            m_playSoundHandle = Play();
    }
    
    //--------------------------------------------------------------------------------------
    void SoundComponent::OnStop()
    {
        m_soundRes.stop(m_playSoundHandle);
        super::OnStop();
    }

    //--------------------------------------------------------------------------------------
    audio::PlaySoundHandle SoundComponent::Play()
    {
        if (m_playSoundHandle)
            Stop();

        m_playSoundHandle = m_soundRes.play(m_soundSettings);
        return m_playSoundHandle;
    }

    //--------------------------------------------------------------------------------------
    bool SoundComponent::Stop()
    {
        return m_soundRes.stop(m_playSoundHandle);
    }
}