#pragma once
#include "core/Singleton/Singleton.h"

namespace vg::renderer
{
    class ParticleSystemInstance;

    class ParticleManager : public core::Singleton<ParticleManager>
    {
    public:
        void    registerParticleSystemInstance      (ParticleSystemInstance * _instance);
        void    unregisterParticleSystemInstance    (ParticleSystemInstance * _instance);

        void    updateSimulation                    ();

    private:
        core::vector<ParticleSystemInstance *>   m_allParticleSystemInstances;
    };
}