#include "GraphicPipelineStateCache.h"
#include "Graphic/GraphicPipelineState.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateCache::clear()
    {
        super::clear();

        for (auto & pair : m_graphicPipelineStateHash)
            VG_SAFE_RELEASE(pair.second);
        m_graphicPipelineStateHash.clear();
    }

    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateCache::reset()
    {
        super::reset();

        memset(&m_graphicPipelineKey, -1, sizeof(GraphicPipelineStateKey));
        m_viewport = uint4(0, 0, 0, 0);
        m_indexBuffer = nullptr;
        m_stencilRef = 0x0;
    }

    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateCache::resetShaders(ShaderKey::File _file)
    {
        vector<GraphicPipelineStateKey> keys;
        for (auto & pair : m_graphicPipelineStateHash)
        {
            const GraphicPipelineStateKey & key = pair.first;
            if (key.m_shaderKey.file == _file)
            {
                VG_SAFE_RELEASE_ASYNC(pair.second);
                keys.push_back(key);
            }
        }
        for (GraphicPipelineStateKey key : keys)
            m_graphicPipelineStateHash.erase(key);
    }

    //--------------------------------------------------------------------------------------
    bool GraphicPipelineStateCache::getGraphicPipelineState(const GraphicPipelineStateKey & _key, GraphicPipelineState *& _graphicPipelineState)
    {
        GraphicPipelineState * pso = nullptr;

        auto it = m_graphicPipelineStateHash.find(_key);
        if (m_graphicPipelineStateHash.end() != it)
        {
            pso = it->second;
        }
        else
        {
            const auto startCreateGraphicsPSO = Timer::getTick();

            pso = GraphicPipelineState::createGraphicPipelineState(_key);
            m_graphicPipelineStateHash[_key] = pso;

            //VG_INFO("[Device] Created Graphics PipelineStateObject 0x%016X in %.2f ms", pso, Timer::getEnlapsedTime(startCreateGraphicsPSO, Timer::getTick()));
        }

        _graphicPipelineState = pso;

        return nullptr != pso;
    }
}