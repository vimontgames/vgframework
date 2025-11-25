#pragma once
#include "renderer/Instance/GraphicInstance.h"
#include "core/Curve/FloatCurve.h"

namespace vg::renderer
{
    struct ParticleEmitterParams
    {
        float               m_spawnRate         = 1.0f;
        float               m_lifeTime          = 5.0f;
        core::u16           m_maxParticleCount  = 1;
        core::float3        m_size             = core::float3(0.1f, 0.1f, 0.1f);
        core::FloatCurve    m_opacity;

        bool operator != (const ParticleEmitterParams & _other) const
        {
            return m_maxParticleCount != _other.m_maxParticleCount || m_spawnRate != _other.m_spawnRate || m_lifeTime != _other.m_lifeTime || any(m_size != _other.m_size) || m_opacity != _other.m_opacity;
        }
    };

    class IParticleSystemInstance : public GraphicInstance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IParticleSystemInstance, GraphicInstance);

        virtual bool SetEmitterCount    (core::uint _count) = 0;
        virtual bool SetEmitterParams   (core::uint _index, const ParticleEmitterParams & _emitterParams) = 0;

        virtual void UpdateTime         (float _dt) = 0;
    };
}