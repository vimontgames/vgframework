#include "SoundComponent.h"
#include "editor/Editor_Consts.h"
#include "core/IGameObject.h"
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

        registerPropertyObject(SoundComponent, m_sounds, "Sounds");

        return true;
    }

    //--------------------------------------------------------------------------------------
    SoundComponent::SoundComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
        m_sounds(_name, this)
    {
        EnableUpdateFlags(UpdateFlags::FixedUpdate | UpdateFlags::Update | UpdateFlags::LateUpdate, false);
    }

    //--------------------------------------------------------------------------------------
    SoundComponent::~SoundComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void SoundComponent::OnPlay()
    {
        super::OnPlay();
        auto & resources = m_sounds.getResources();
        for (auto i = 0; i < resources.size(); ++i)
            resources[i].OnPlay();
    }
    
    //--------------------------------------------------------------------------------------
    void SoundComponent::OnStop()
    {
        auto & resources = m_sounds.getResources();
        for (auto i = 0; i < resources.size(); ++i)
            resources[i].OnStop();
        super::OnStop();
    }

    //--------------------------------------------------------------------------------------
    audio::PlaySoundHandle SoundComponent::Play(core::uint _index)
    {    
        auto & resources = m_sounds.getResources();
        if (_index < resources.size())
        {
            return resources[_index].play();
        }
        else
        {
            VG_WARNING("[Sound] Could not play sound at index %u in GameObject \"%s\"", _index, GetGameObject()->GetName().c_str());
            return (audio::PlaySoundHandle)0;
        }
    }
    
    //--------------------------------------------------------------------------------------
    bool SoundComponent::Stop(core::uint _index)
    {
        auto & resources = m_sounds.getResources();
        if (_index < resources.size())
        {
            return resources[_index].stop();
        }
        else
        {
            VG_WARNING("[Sound] Could not play stop at index %u in GameObject \"%s\"", _index, GetGameObject()->GetName().c_str());
            return false;
        }
    }
}