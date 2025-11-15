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

        const renderer::ParticleEmitterParams & getEmitterParams() const { return m_params; }
        const MaterialResource & getMaterialResource() const { return m_materialResource; }

    private:
        renderer::ParticleEmitterParams     m_params;
        MaterialResource                    m_materialResource;
    };
}