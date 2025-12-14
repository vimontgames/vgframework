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
        registerPropertyGroupBegin(ParticleEmitterDesc, "Spawn");
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
        registerPropertyGroupBegin(ParticleEmitterDesc, "Size");
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
        registerPropertyGroupBegin(ParticleEmitterDesc, "Velocity");
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
        registerPropertyGroupBegin(ParticleEmitterDesc, "Color");
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
        registerPropertyGroupBegin(ParticleEmitterDesc, "Opacity");
        {
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

        registerPropertyGroupBegin(ParticleEmitterDesc, "FlipBook");
        {
            registerProperty(ParticleEmitterDesc, m_params.m_framerate, "Framerate");
            setPropertyDescription(ParticleEmitterDesc, m_params.m_framerate, "Number of frames displayed per second");
            setPropertyRange(ParticleEmitterDesc, m_params.m_framerate, uint2(1.0f, 60.0f));
        }
        registerPropertyGroupEnd(ParticleEmitterDesc);

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

        m_params.m_colorOverLifeTime.SetCurveValueType(CurveValueType::Color);
    }

    ////--------------------------------------------------------------------------------------
    //ParticleEmitterDesc::ParticleEmitterDesc(const ParticleEmitterDesc & _other) :
    //    super(_other)
    //{
    //    m_params = _other.m_params;
    //    m_materialResource.setParent(this); // makes crash!
    //
    //    // swap client pointers in manager instead?
    //
    //    //string path = _other.m_materialResource.GetResourcePath();
    //    ////Kernel::getResourceManager()->UnloadResource((IResource*)&_other.m_materialResource, path);
    //    //
    //    ////m_materialResource.SetParent(this);
    //    ////auto objectUID = _other.GetUID();
    //    ////auto matResUID = _other.m_materialResource.GetUID();
    //    ////string path = _other.m_materialResource.GetResourcePath();
    //    //((ParticleEmitterDesc &)_other).m_materialResource.SetResourcePath("");
    //    ////((ParticleEmitterDesc &)_other).UnregisterUID();
    //    ////((ParticleEmitterDesc &)_other.m_materialResource).UnregisterUID();
    //    ////SetUID(objectUID);
    //    ////m_materialResource.SetResourcePath(path);
    //    ////m_materialResource.SetUID(matResUID);
    //    //
    //    //m_materialResource.SetParent(this);
    //    //m_materialResource.SetObject(_other.m_materialResource.GetObject());
    //    //_other.m_materialResource.GetObject()->AddRef();
    //    ////m_materialResource.SetResourcePath(path); // copy path without reload and add client manually?
    //}

    //--------------------------------------------------------------------------------------
    ParticleEmitterDesc::~ParticleEmitterDesc()
    {
     
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