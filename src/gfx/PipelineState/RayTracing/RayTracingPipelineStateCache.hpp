#include "RayTracingPipelineStateCache.h"
#include "RayTracingPipelineState.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    void RayTracingPipelineStateCache::clear()
    {
        super::clear();

        for (auto & pair : m_rayTracingPipelineStateHash)
            VG_SAFE_RELEASE(pair.second);
        m_rayTracingPipelineStateHash.clear();
    }

    //--------------------------------------------------------------------------------------
    void RayTracingPipelineStateCache::reset()
    {
        super::reset();

        memset(&m_rayTracingPipelineKey, -1, sizeof(RayTracingPipelineStateKey));
    }

    //--------------------------------------------------------------------------------------
    void RayTracingPipelineStateCache::resetShaders(ComputeShaderKey::File _file)
    {
        vector<RayTracingPipelineStateKey> rayTracingKeys;
        for (auto & pair : m_rayTracingPipelineStateHash)
        {
            const RayTracingPipelineStateKey & rayTracingKey = pair.first;
            if (rayTracingKey.m_rayTracingShaderKey.file == _file)
            {
                VG_SAFE_RELEASE_ASYNC(pair.second);
                rayTracingKeys.push_back(rayTracingKey);
            }
        }
        for (RayTracingPipelineStateKey rayTracingKey : rayTracingKeys)
            m_rayTracingPipelineStateHash.erase(rayTracingKey);
    }

    //--------------------------------------------------------------------------------------
    bool RayTracingPipelineStateCache::getRayTracingPipelineState(const RayTracingPipelineStateKey & _key, RayTracingPipelineState *& _rayTracingPipelineState)
    {
        RayTracingPipelineState * pso = nullptr;

        auto it = m_rayTracingPipelineStateHash.find(_key);
        if (m_rayTracingPipelineStateHash.end() != it)
        {
            pso = it->second;
        }
        else
        {
            const auto startCreatePSO = Timer::getTick();

            pso = RayTracingPipelineState::createRayTracingPipelineState(_key);
            m_rayTracingPipelineStateHash[_key] = pso;

            if (pso)
                VG_INFO("[Device] Created RayTracing PSO 0x%016X in %.2f ms", pso, Timer::getEnlapsedTime(startCreatePSO, Timer::getTick()));
        }

        _rayTracingPipelineState = pso;

        return nullptr != pso;
    }
}