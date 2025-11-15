#include "ParticleEmitterList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(ParticleEmitterList, "Particle EmitterList List");

    //--------------------------------------------------------------------------------------
    ParticleEmitterList::ParticleEmitterList(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    ParticleEmitterList::~ParticleEmitterList()
    {

    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}