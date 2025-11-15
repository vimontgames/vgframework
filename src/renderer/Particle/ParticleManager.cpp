#include "renderer/Precomp.h"
#include "ParticleManager.h"
#include <algorithm>
#include "renderer/Instance/ParticleSystem/ParticleSystemInstance.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ParticleManager::registerParticleSystemInstance(ParticleSystemInstance * _instance)
    {
        auto it = find(m_allParticleSystemInstances.begin(), m_allParticleSystemInstances.end(), _instance);
        VG_ASSERT(m_allParticleSystemInstances.end() == it);
        m_allParticleSystemInstances.push_back(_instance);
    }

    //--------------------------------------------------------------------------------------
    void ParticleManager::unregisterParticleSystemInstance(ParticleSystemInstance * _instance)
    {
        auto it = find(m_allParticleSystemInstances.begin(), m_allParticleSystemInstances.end(), _instance);
        VG_ASSERT(m_allParticleSystemInstances.end() != it);
        if (m_allParticleSystemInstances.end() != it)
            m_allParticleSystemInstances.erase(it);
    }

    //--------------------------------------------------------------------------------------
    void ParticleManager::updateSimulation()
    {
        VG_PROFILE_CPU("UpdateParticleSimulation");

        for (uint i = 0; i < m_allParticleSystemInstances.size(); ++i)
        {
            ParticleSystemInstance * instance = m_allParticleSystemInstances[i];
            instance->updateSimulation();
        }
    }
}