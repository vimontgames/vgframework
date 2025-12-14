#pragma once
#include "renderer/Instance/GraphicInstance.h"
#include "core/Curve/Float1Curve.h"
#include "core/Curve/Float3Curve.h"

namespace vg::renderer
{
    vg_enum_class(vg::renderer, ParticleValueType, vg::core::u8,
        Constant = 0,
        Curve
    );

    struct ParticleEmitterParams
    {
        // State
        bool                m_play                  = false;
        bool                m_spawn                 = false;

        // Spawning
        float               m_spawnRate             = 1.0f;
        core::u16           m_maxParticleCount      = 1;

        // Life
        float               m_lifeTime              = 3.0f;
        bool                m_neverDie              = false;

        // Size
        ParticleValueType   m_sizeValueType         = ParticleValueType::Constant;
        core::float3        m_constantSize          = core::float3(1.0f, 1.0f, 1.0f);
        core::Float3Curve   m_sizeOverLifeTime;

        // Velocity
        ParticleValueType   m_velocityValueType     = ParticleValueType::Constant;
        core::float3        m_constantVelocity      = core::float3(0.0f, 0.0f, 0.0f);
        core::Float3Curve   m_velocityOverLifeTime;

        // Color
        ParticleValueType   m_colorValueType        = ParticleValueType::Constant;
        core::float3        m_constantColor         = core::float3(1.0f, 1.0f, 1.0f);
        core::Float3Curve   m_colorOverLifeTime;

        // Opacity
        ParticleValueType   m_opacityValueType      = ParticleValueType::Constant;
        float               m_constantOpacity       = 1.0f;
        core::Float1Curve   m_opacityOverLifeTime;

        // FlipBook
        float               m_framerate             = 1.0f;

        bool operator != (const ParticleEmitterParams & _other) const
        {
                // State
            return m_play != _other.m_play 
                || m_spawn != _other.m_spawn 

                // Spawning
                || m_maxParticleCount != _other.m_maxParticleCount 
                || m_spawnRate != _other.m_spawnRate 

                // Life
                || m_lifeTime != _other.m_lifeTime 
                || m_neverDie != _other.m_neverDie

                // Size
                || m_sizeValueType != _other.m_sizeValueType
                || any(m_constantSize != _other.m_constantSize)
                //
                // Velocity
                || m_velocityValueType != _other.m_velocityValueType
                || any(m_constantVelocity != _other.m_constantVelocity)
                
                // Color
                || m_colorValueType != _other.m_colorValueType
                || any(m_constantColor != _other.m_constantColor)
                || m_colorOverLifeTime != _other.m_colorOverLifeTime
                
                // Opacity
                || m_opacityValueType != _other.m_opacityValueType
                || m_constantOpacity != _other.m_constantOpacity
                || m_opacityOverLifeTime != _other.m_opacityOverLifeTime

                // FlipBook
                || m_framerate != _other.m_framerate;
        }
    };

    class IParticleSystemInstance : public GraphicInstance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IParticleSystemInstance, GraphicInstance);

        virtual bool SetEmitterCount    (core::uint _count) = 0;
        virtual bool SetEmitterParams   (core::uint _index, const ParticleEmitterParams & _emitterParams) = 0;
        virtual void ResetEmitter(core::uint _index) = 0;

        virtual void UpdateTime         (float _dt) = 0;
        virtual void UpdateEmitterTime  (core::uint _index, float _dt) = 0;
    };
}