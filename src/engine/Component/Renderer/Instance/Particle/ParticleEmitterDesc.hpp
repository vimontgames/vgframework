#include "ParticleEmitterDesc.h"

#include <array>

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
    bool ParticleEmitterDesc::playParticleEmitter(IObject * _object)
    {
        return ((ParticleEmitterDesc *)_object)->Play();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::pauseParticleEmitter(IObject * _object)
    {
        return ((ParticleEmitterDesc *)_object)->Pause();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::stopParticleEmitter(IObject * _object)
    {
        return ((ParticleEmitterDesc *)_object)->Stop();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyCallbackEx(ParticleEmitterDesc, playParticleEmitter, editor::style::icon::Play, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleEmitterDesc, playParticleEmitter, "Play emitter");

        registerPropertyCallbackEx(ParticleEmitterDesc, pauseParticleEmitter, editor::style::icon::Pause, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleEmitterDesc, pauseParticleEmitter, "Pause emitter");

        registerPropertyCallbackEx(ParticleEmitterDesc, stopParticleEmitter, editor::style::icon::Stop, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleEmitterDesc, stopParticleEmitter, "Stop emitter");        

        setPropertyFlag(ParticleEmitterDesc, m_name, PropertyFlags::Hidden, false); // TODO: show icons *BEFORE* name would look better

        registerPropertyEx(ParticleEmitterDesc, m_params.m_play, "Play", PropertyFlags::Transient | PropertyFlags::Debug);
        registerPropertyEx(ParticleEmitterDesc, m_params.m_spawn, "Spawn", PropertyFlags::Transient | PropertyFlags::Debug);

        registerProperty(ParticleEmitterDesc, m_params.m_spawnRate, "Spawn rate");
        setPropertyDescription(ParticleEmitterDesc, m_params.m_spawnRate, "Number of particles spawn per second");
        setPropertyRange(ParticleEmitterDesc, m_params.m_spawnRate, uint2(0.0f, 60.0f));

        registerOptionalProperty(ParticleEmitterDesc, m_params.m_useLifeTime, m_params.m_lifeTime, "Life time");
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

        registerProperty(ParticleEmitterDesc, m_params.m_colorAndOpacityCurve, "Color & Alpha");
        setPropertyDescription(ParticleEmitterDesc, m_params.m_colorAndOpacityCurve, "Color and alpha over lifetime");

        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::FlipBook);
        {
            registerProperty(ParticleEmitterDesc, m_params.m_framerate, "Framerate");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_framerate, "Number of frames displayed per second");
            setPropertyRange(ParticleEmitterDesc, m_params.m_framerate, uint2(1.0f, 60.0f));
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::Material);
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

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::Play() 
    {
        ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent());
        auto index = particleComp->getEmitterIndex(this);
        if (-1 != index)
        {
            m_params.m_play = true;
            m_params.m_spawn = true;

            particleComp->UpdateEmitter(index);
            particleComp->ResetEmitter(index);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::Pause()
    {
        ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent());
        auto index = particleComp->getEmitterIndex(this);
        if (-1 != index)
        {
            m_params.m_play = !m_params.m_play;

            particleComp->UpdateEmitter(index);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::Stop()
    {
        ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent());
        auto index = particleComp->getEmitterIndex(this);
        if (-1 != index)
        {
            m_params.m_spawn = false;

            particleComp->UpdateEmitter(index);

            return true;
        }

        return false;
    }
}