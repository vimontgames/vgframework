#pragma once

#include "core/Object/ObjectList.h"
#include "ParticleEmitterDesc.h"

namespace vg::engine
{
    class ParticleEmitterList : public core::ObjectList<ParticleEmitterDesc>
    {
    public:
        VG_CLASS_DECL(ParticleEmitterList, ObjectList<ParticleEmitterDesc>);

        ParticleEmitterList(const core::string & _name, core::IObject * _parent);
        ~ParticleEmitterList();
    };
}