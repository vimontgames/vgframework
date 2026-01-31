#include "audio/Precomp.h"
#include "AudioOptions.h"
#include "audio/Audio.h"

using namespace vg::core;

namespace vg::audio
{
    VG_REGISTER_CLASS(AudioOptions, "Audio Options");

    //--------------------------------------------------------------------------------------
    bool AudioOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);


        return true;
    }

    //--------------------------------------------------------------------------------------
    AudioOptions::AudioOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        SetFile("Audio.xml");
        Load(false);
    }

    //--------------------------------------------------------------------------------------
    void AudioOptions::OnLoad()
    {

    }

    //--------------------------------------------------------------------------------------
    void AudioOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }
}