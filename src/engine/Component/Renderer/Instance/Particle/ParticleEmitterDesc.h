#pragma once

#include "core/Object/Object.h"
#include "engine/Resource/Material/MaterialResource.h"
#include "renderer/Instance/ParticleSystem/ParticleSystemInstance.h"

namespace vg::engine
{
    class ParticleEmitterDesc final : public core::Object
    {
    public:
        VG_CLASS_DECL(ParticleEmitterDesc, core::Object);

        ParticleEmitterDesc(const vg::core::string & _name = "", vg::core::IObject * _parent = nullptr);

        void OnLoad() final override;

        bool Play();
        bool Pause();
        bool Stop();

        const renderer::ParticleEmitterParams & getEmitterParams() const { return m_params; }
        renderer::ParticleEmitterParams & getEmitterParams() { return m_params; }
        const MaterialResource & getMaterialResource() const { return m_materialResource; }

        static bool playParticleEmitter(IObject * _object);
        static bool pauseParticleEmitter(IObject * _object);
        static bool stopParticleEmitter(IObject * _object);

    private:
        renderer::ParticleEmitterParams     m_params;
        MaterialResource                    m_materialResource;
    };
}