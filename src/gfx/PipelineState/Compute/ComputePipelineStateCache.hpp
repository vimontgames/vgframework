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
    void ComputePipelineStateCache::resetComputePipelineStates(ComputeShaderKey::File _file)
    {
        resetComputePipelineStatesImpl(m_computePipelineStateHash, _file);
    }

    //--------------------------------------------------------------------------------------
    void ComputePipelineStateCache::resetComputePipelineStatesImpl(ComputePipelineStateHash & _computePipelineStateHash, ComputeShaderKey::File _file)
    {
        vector<ComputePipelineStateKey> computeKeys;
        for (auto & pair : _computePipelineStateHash)
        {
            const ComputePipelineStateKey & computeKey = pair.first;
            if (computeKey.m_computeShaderKey.file == _file)
            {
                VG_SAFE_RELEASE_ASYNC(pair.second);
                computeKeys.push_back(computeKey);
            }
        }
        for (ComputePipelineStateKey computeKey : computeKeys)
            _computePipelineStateHash.erase(computeKey);
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
            pso = ShaderManager::get()->createComputePipelineState(_key);
            m_computePipelineStateHash[_key] = pso;
        }

        _computePipelineState = pso;

        return nullptr != pso;
    }
}