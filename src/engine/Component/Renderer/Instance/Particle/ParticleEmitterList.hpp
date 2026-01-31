#include "ParticleEmitterList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_CLASS(ParticleEmitterList, "Particle EmitterList List");

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}