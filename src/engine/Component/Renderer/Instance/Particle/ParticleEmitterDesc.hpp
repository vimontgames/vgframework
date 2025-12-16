#include "ParticleEmitterDesc.h"
#include "core/string/string.h"
#include "core/IResourceManager.h"

#include <array>

using namespace vg::core;
using namespace vg::renderer;

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
    bool IsSize2DConstantHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_sizeValueType != ParticleValueType::Constant || true; // TODO: particle meshes
    }

    //--------------------------------------------------------------------------------------
    bool IsSize3DConstantHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_sizeValueType != ParticleValueType::Constant || false; // TODO: particle meshes
    }

    //--------------------------------------------------------------------------------------
    bool IsSizeCurveHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_sizeValueType != ParticleValueType::Curve;
    }

    //--------------------------------------------------------------------------------------
    bool IsVelocityConstantHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_velocityValueType != ParticleValueType::Constant;
    }

    //--------------------------------------------------------------------------------------
    bool IsVelocityCurveHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_velocityValueType != ParticleValueType::Curve;
    }

    //--------------------------------------------------------------------------------------
    bool IsConstantColorHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_colorValueType != ParticleValueType::Constant;
    }

    //--------------------------------------------------------------------------------------
    bool IsColorCurveHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_colorValueType != ParticleValueType::Curve;
    }    

    //--------------------------------------------------------------------------------------
    bool IsConstantOpacityHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_opacityValueType != ParticleValueType::Constant;
    }

    //--------------------------------------------------------------------------------------
    bool IsOpacityCurveHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const ParticleEmitterDesc * desc = VG_SAFE_STATIC_CAST(const ParticleEmitterDesc, _object);
        return desc->getEmitterParams().m_opacityValueType != ParticleValueType::Curve;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Controls
        registerPropertyCallbackEx(ParticleEmitterDesc, playParticleEmitter, editor::style::icon::Play, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleEmitterDesc, playParticleEmitter, "Play emitter");

        registerPropertyCallbackEx(ParticleEmitterDesc, pauseParticleEmitter, editor::style::icon::Pause, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleEmitterDesc, pauseParticleEmitter, "Pause emitter");

        registerPropertyCallbackEx(ParticleEmitterDesc, stopParticleEmitter, editor::style::icon::Stop, PropertyFlags::SingleLine);
        setPropertyDescription(ParticleEmitterDesc, stopParticleEmitter, "Stop emitter");        

        setPropertyFlag(ParticleEmitterDesc, m_name, PropertyFlags::Hidden, false); // TODO: show icons *BEFORE* name would look better

        // State
        registerPropertyEx(ParticleEmitterDesc, m_params.m_play, "Play", PropertyFlags::Transient | PropertyFlags::Debug);
        registerPropertyEx(ParticleEmitterDesc, m_params.m_spawn, "Spawn", PropertyFlags::Transient | PropertyFlags::Debug);

        // Spawning
        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::Life);
        {
            registerProperty(ParticleEmitterDesc, m_params.m_spawnRate, "Spawn rate");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_spawnRate, "Number of particles spawn per second");
            setPropertyRange(ParticleEmitterDesc, m_params.m_spawnRate, uint2(0.0f, 60.0f));

            registerProperty(ParticleEmitterDesc, m_params.m_lifeTime, "Life duration");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_lifeTime, "Life time of a particle");
            setPropertyRange(ParticleEmitterDesc, m_params.m_lifeTime, uint2(0.0f, 60.0f));    

            registerProperty(ParticleEmitterDesc, m_params.m_neverDie, "Never die");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_neverDie, "Particles won't be killed after life");            

            registerProperty(ParticleEmitterDesc, m_params.m_maxParticleCount, "Max Particles");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_maxParticleCount, "Maximum amount of particles this emitter can spawn");
            setPropertyRange(ParticleEmitterDesc, m_params.m_maxParticleCount, uint2(0, 1024));
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        // Size 
        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::Size);
        {
            registerPropertyEnum(ParticleEmitterDesc, ParticleValueType, m_params.m_sizeValueType, "Type");

            registerProperty(ParticleEmitterDesc, m_params.m_constantSize, "Size");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_constantSize, "Particle size (3D)");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_constantSize, IsSize2DConstantHidden)

            registerPropertyAlias(ParticleEmitterDesc, float2, m_params.m_constantSize, "Size");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_constantSize, "Particle size (2D)");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_constantSize, IsSize3DConstantHidden)

            registerPropertyEx(ParticleEmitterDesc, m_params.m_sizeOverLifeTime, "Size", PropertyFlags::Flatten);
            setPropertyDescription(ParticleEmitterDesc, m_params.m_sizeOverLifeTime, "Size over lifetime");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_sizeOverLifeTime, IsSizeCurveHidden)
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        // Velocity
        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::Speed);
        {
            registerPropertyEnum(ParticleEmitterDesc, ParticleValueType, m_params.m_velocityValueType, "Type");

            registerProperty(ParticleEmitterDesc, m_params.m_constantVelocity, "Velocity");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_constantVelocity, "Initial velocity");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_constantVelocity, IsVelocityConstantHidden)

            registerPropertyEx(ParticleEmitterDesc, m_params.m_velocityOverLifeTime, "Opacity", PropertyFlags::Flatten);
            setPropertyDescription(ParticleEmitterDesc, m_params.m_velocityOverLifeTime, "Velocity over lifetime");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_velocityOverLifeTime, IsVelocityCurveHidden)
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        // Color
        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::Color);
        {
            registerPropertyEnum(ParticleEmitterDesc, ParticleValueType, m_params.m_colorValueType, "Type");

            registerPropertyEx(ParticleEmitterDesc, m_params.m_constantColor, "Color", PropertyFlags::Color);
            setPropertyDescription(ParticleEmitterDesc, m_params.m_constantColor, "Initial color");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_constantColor, IsConstantColorHidden)

            registerPropertyEx(ParticleEmitterDesc, m_params.m_colorOverLifeTime, "Color", PropertyFlags::Flatten);
            setPropertyDescription(ParticleEmitterDesc, m_params.m_colorOverLifeTime, "Color over lifetime");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_colorOverLifeTime, IsColorCurveHidden)
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        // Opacity
        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::Opacity);
        {
            registerProperty(ParticleEmitterDesc, m_params.m_cullZeroAlpha, "Cull zero alpha");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_cullZeroAlpha, "Do not render particles with an alpha of zero");

            registerPropertyEnum(ParticleEmitterDesc, ParticleValueType, m_params.m_opacityValueType, "Type");

            registerProperty(ParticleEmitterDesc, m_params.m_constantOpacity, "Opacity");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_constantOpacity, "Initial opacity");
            setPropertyRange(ParticleEmitterDesc, m_params.m_constantOpacity, float2(0, 1));
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_constantOpacity, IsConstantOpacityHidden)

            registerPropertyEx(ParticleEmitterDesc, m_params.m_opacityOverLifeTime, "Opacity", PropertyFlags::Flatten);
            setPropertyDescription(ParticleEmitterDesc, m_params.m_opacityOverLifeTime, "Opacity over lifetime");
            setPropertyHiddenCallback(ParticleEmitterDesc, m_params.m_opacityOverLifeTime, IsOpacityCurveHidden)
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::FlipBook);
        {
            registerProperty(ParticleEmitterDesc, m_params.m_framerate, "Framerate");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_framerate, "Number of frames displayed per second");
            setPropertyRange(ParticleEmitterDesc, m_params.m_framerate, uint2(1.0f, 60.0f));
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

        registerPropertyGroupBegin(ParticleEmitterDesc, editor::style::label::particle::Material);
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

        m_params.m_colorOverLifeTime.SetCurveValueType(CurveValueType::Color);
    }

    //--------------------------------------------------------------------------------------
    ParticleEmitterDesc::~ParticleEmitterDesc()
    {
        // We can't get index when removing element from vector
        //if (ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent()))
        //{
        //    uint index = particleComp->getEmitterIndex(this);
        //    particleComp->onEmitterRemoved(index);
        //}

        // So reset the particle system instead :(
        if (ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent()))
            particleComp->Reset();
    }

    //--------------------------------------------------------------------------------------
    void ParticleEmitterDesc::Swap(IObject * _other)
    {
        super::Swap(_other);

        ParticleEmitterDesc * other = VG_SAFE_STATIC_CAST(ParticleEmitterDesc, _other);
        m_params.Swap(other->m_params);

        m_materialResource.Swap(&other->m_materialResource);

        // Solution 1: Reset both emitters
        // Reset();
        // other->Reset();

        // Solution 2: Reorder
        if (ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent()))
        {
            uint indexA = particleComp->getEmitterIndex(this);
            uint indexB = particleComp->getEmitterIndex(other);
            particleComp->onEmittersSwap(indexA, indexB);
        }
    }  

    //--------------------------------------------------------------------------------------
    void ParticleEmitterDesc::OnLoad()
    {
        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    bool ParticleEmitterDesc::Reset()
    {
        ParticleComponent * particleComp = ((ParticleComponent *)getParent()->GetParent());
        auto index = particleComp->getEmitterIndex(this);
        if (-1 != index)
        {
            particleComp->ResetEmitter(index);

            return true;
        }

        return false;
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