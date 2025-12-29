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
        bool                m_cullZeroAlpha         = true;
        ParticleValueType   m_opacityValueType      = ParticleValueType::Constant;
        float               m_constantOpacity       = 1.0f;
        core::Float1Curve   m_opacityOverLifeTime;

        // FlipBook
        float               m_framerate             = 1.0f;
        bool                m_randomStartFrame      = false;

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
                || m_sizeOverLifeTime != _other.m_sizeOverLifeTime

                // Velocity
                || m_velocityValueType != _other.m_velocityValueType
                || any(m_constantVelocity != _other.m_constantVelocity)
                || m_velocityOverLifeTime != _other.m_velocityOverLifeTime

                // Color
                || m_colorValueType != _other.m_colorValueType
                || any(m_constantColor != _other.m_constantColor)
                || m_colorOverLifeTime != _other.m_colorOverLifeTime

                // Opacity
                || m_cullZeroAlpha != _other.m_cullZeroAlpha
                || m_opacityValueType != _other.m_opacityValueType
                || m_constantOpacity != _other.m_constantOpacity
                || m_opacityOverLifeTime != _other.m_opacityOverLifeTime

                // FlipBook
                || m_framerate != _other.m_framerate
                || m_randomStartFrame != _other.m_randomStartFrame;
        }

        // Manual swap is needed because of curves so that they keep the same after swap
        void Swap(ParticleEmitterParams & _other)
        {
            // State
            core::swap(m_play, _other.m_play);
            core::swap(m_spawn, _other.m_spawn);

            // Spawning
            core::swap(m_maxParticleCount, _other.m_maxParticleCount);
            core::swap(m_spawnRate, _other.m_spawnRate);
 
            // Life
            core::swap(m_lifeTime, _other.m_lifeTime);
            core::swap(m_neverDie, _other.m_neverDie);
     
            // Size
            core::swap(m_sizeValueType, _other.m_sizeValueType);
            core::swap(m_constantSize, _other.m_constantSize);
            m_sizeOverLifeTime.Swap(&_other.m_sizeOverLifeTime);

            // Velocity
            core::swap(m_velocityValueType, _other.m_velocityValueType);
            core::swap(m_constantVelocity, _other.m_constantVelocity);
            m_velocityOverLifeTime.Swap(&_other.m_velocityOverLifeTime);

            // Color
            core::swap(m_colorValueType, _other.m_colorValueType);
            core::swap(m_constantColor, _other.m_constantColor);
            m_colorOverLifeTime.Swap(&_other.m_colorOverLifeTime);

            // Opacity
            core::swap(m_cullZeroAlpha, _other.m_cullZeroAlpha);
            core::swap(m_opacityValueType, _other.m_opacityValueType);
            core::swap(m_constantOpacity, _other.m_constantOpacity);
            m_opacityOverLifeTime.Swap(&_other.m_opacityOverLifeTime);

            // FlipBook
            core::swap(m_framerate, _other.m_framerate);
            core::swap(m_randomStartFrame, _other.m_randomStartFrame);
        }
    };

    class IParticleSystemInstance : public GraphicInstance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IParticleSystemInstance, GraphicInstance);

        virtual bool SetWorldSpace      (bool _isWorldSpace) = 0;

        virtual bool SetEmitterCount    (core::uint _count) = 0;
        virtual bool SetEmitterParams   (core::uint _index, const ParticleEmitterParams & _emitterParams) = 0;
        virtual void ResetEmitter       (core::uint _index) = 0;

        virtual void UpdateTime         (float _dt) = 0;
        virtual void UpdateEmitterTime  (core::uint _index, float _dt) = 0;

        virtual bool OnEmittersSwap     (core::uint _indexA, core::uint _indexB) = 0;
        virtual bool OnEmittersRemoved  (core::uint _index) = 0;
    };
}