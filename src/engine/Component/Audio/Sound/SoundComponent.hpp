#include "SoundComponent.h"
#include "editor/Editor_Consts.h"
#include "core/Object/Update.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(SoundComponent, "Sound", "Audio", "Audio sound component", editor::style::icon::Sound, 0);

    //--------------------------------------------------------------------------------------
    bool SoundComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyResource(SoundComponent, m_soundRes, "Path");

        return true;
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
    audio::PlaySoundHandle SoundComponent::Play()
    {
        return m_soundRes.play();
    }
}