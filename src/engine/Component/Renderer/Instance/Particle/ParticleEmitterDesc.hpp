#include "ParticleEmitterDesc.h"

using namespace vg::core;

//#define registerPropertyAsEx(className, as, propertyName, displayName, flags)   _desc.RegisterProperty(#className, #propertyName, (as*)(&((className*)(nullptr))->propertyName), displayName, flags)
//#define registerPropertyAs(className, as, propertyName, displayName)            registerPropertyAsEx(className, as, propertyName, displayName, vg::core::PropertyFlags::None)

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(ParticleEmitterDesc, "Particle emitter descriptor");

    //--------------------------------------------------------------------------------------
    void * ResizeParticleEmitterDescVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<ParticleEmitterDesc>*)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(ParticleEmitterDesc, m_name, PropertyFlags::Hidden, false);

        registerProperty(ParticleEmitterDesc, m_params.m_spawnRate, "Spawn rate");
        setPropertyDescription(ParticleEmitterDesc, m_params.m_spawnRate, "Number of particles spawn per second");
        setPropertyRange(ParticleEmitterDesc, m_params.m_spawnRate, uint2(0.0f, 60.0f));

        registerProperty(ParticleEmitterDesc, m_params.m_lifeTime, "Life time");
        setPropertyDescription(ParticleEmitterDesc, m_params.m_lifeTime, "Life time of a particle");
        setPropertyRange(ParticleEmitterDesc, m_params.m_lifeTime, uint2(0.0f, 60.0f));        

        registerProperty(ParticleEmitterDesc, m_params.m_maxParticleCount, "Max Particles");
        setPropertyDescription(ParticleEmitterDesc, m_params.m_maxParticleCount, "Maximum amount of particles this emitter can spawn");
        setPropertyRange(ParticleEmitterDesc, m_params.m_maxParticleCount, uint2(0, 1024));

        // TODO: float3 for mesh particles but float2 for billboard particles?
        registerProperty(ParticleEmitterDesc, m_params.m_size, "Size");
        setPropertyDescription(ParticleEmitterDesc, m_params.m_size, "Particle size (2D)");
        setPropertyRange(ParticleEmitterDesc, m_params.m_size, uint2(0.0f, 1.0f));

        //registerPropertyAsEx(ParticleEmitterDesc, float2, m_params.m_size, "Size", PropertyFlags::Transient);
        //setPropertyDescription(ParticleEmitterDesc, m_params.m_size, "Particle size");
        //setPropertyRange(ParticleEmitterDesc, m_params.m_size, uint2(0.0f, 1.0f));

        registerPropertyGroupBegin(ParticleEmitterDesc, "Material");
        {
            registerPropertyResource(ParticleEmitterDesc, m_materialResource, "Material");
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        registerResizeVectorFunc(ParticleEmitterDesc, ResizeParticleEmitterDescVector);

        return true;
    }

    //--------------------------------------------------------------------------------------
    ParticleEmitterDesc::ParticleEmitterDesc(const vg::core::string & _name, vg::core::IObject * _parent) : 
        super(_name, _parent)
    {
        m_materialResource.SetParent(this);
        m_materialResource.RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    void ParticleEmitterDesc::OnLoad()
    {
        RegisterUID();
    }
}