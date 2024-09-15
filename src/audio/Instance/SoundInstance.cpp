#include "audio/Precomp.h"
#include "SoundInstance.h"

using namespace vg::core;

namespace vg::audio
{
    //--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS(SoundInstance, "Sound instance");

    //--------------------------------------------------------------------------------------
    bool SoundInstance::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    SoundInstance::SoundInstance(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        
    }

    //--------------------------------------------------------------------------------------
    SoundInstance::~SoundInstance()
    {
        VG_SAFE_DELETE(m_slAudioSource);
    }

    //--------------------------------------------------------------------------------------
    bool SoundInstance::TryGetAABB(AABB & _aabb) const
    {
        return false;
    }
}