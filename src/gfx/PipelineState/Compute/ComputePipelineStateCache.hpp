#include "ComputePipelineStateCache.h"
#include "ComputePipelineState.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    void ComputePipelineStateCache::clear()
    {
        super::clear();

        for (auto & pair : m_computePipelineStateHash)
            VG_SAFE_RELEASE(pair.second);
        m_computePipelineStateHash.clear();
    }

    //--------------------------------------------------------------------------------------
    void ComputePipelineStateCache::reset()
    {
        super::reset();

        memset(&m_computePipelineKey, -1, sizeof(ComputePipelineStateKey));
    }

    //--------------------------------------------------------------------------------------
    void ComputePipelineStateCache::resetShaders(ComputeShaderKey::File _file)
    {
        vector<ComputePipelineStateKey> computeKeys;
        for (auto & pair : m_computePipelineStateHash)
        {
            const ComputePipelineStateKey & computeKey = pair.first;
            if (computeKey.m_computeShaderKey.file == _file)
            {
                VG_SAFE_RELEASE_ASYNC(pair.second);
                computeKeys.push_back(computeKey);
            }
        }
        for (ComputePipelineStateKey computeKey : computeKeys)
            m_computePipelineStateHash.erase(computeKey);
    }

    //--------------------------------------------------------------------------------------
    bool ComputePipelineStateCache::getComputePipelineState(const ComputePipelineStateKey & _key, ComputePipelineState *& _computePipelineState)
    {
        ComputePipelineState * pso = nullptr;

        auto it = m_computePipelineStateHash.find(_key);
        if (m_computePipelineStateHash.end() != it)
        {
            pso = it->second;
        }
        else
        {
            core::lock_guard<mutex> lock(ShaderManager::get()->getComputePipelineStateMutex());

            const auto startCreateComputePSO = Timer::getTick();

            pso = ComputePipelineState::createComputePipelineState(_key);
            m_computePipelineStateHash[_key] = pso;

            //VG_INFO("[Device] Created Compute PipelineStateObject 0x%016X in %.2f ms", pso, Timer::getEnlapsedTime(startCreateComputePSO, Timer::getTick()));
        }

        _computePipelineState = pso;

        return nullptr != pso;
    }
}