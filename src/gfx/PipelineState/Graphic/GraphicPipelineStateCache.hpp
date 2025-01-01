#include "GraphicPipelineStateCache.h"
#include "Graphic/GraphicPipelineState.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    GraphicPipelineStateCache::GraphicPipelineStateCache() : 
        PipelineStateCache()
    {

    }

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

        m_graphicPipelineKey.reset();
        m_viewport = uint4(0, 0, 0, 0);
        m_indexBuffer = nullptr;
        m_stencilRef = 0x0;
    }

    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateCache::resetGraphicPipelineStates(ShaderKey::File _file)
    {
        resetGraphicPipelineStatesImpl(m_graphicPipelineStateHash, _file);
    }

    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateCache::resetGraphicPipelineStatesImpl(GraphicPipelineStateHash & _graphicPipelineStateHash, ShaderKey::File _file)
    {
        vector<GraphicPipelineStateKey> keys;
        for (auto & pair : _graphicPipelineStateHash)
        {
            const GraphicPipelineStateKey & key = pair.first;
            if (key.m_shaderKey.file == _file)
            {
                VG_SAFE_RELEASE_ASYNC(pair.second);
                keys.push_back(key);
            }
        }
        for (GraphicPipelineStateKey key : keys)
            _graphicPipelineStateHash.erase(key);
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
            pso = ShaderManager::get()->createGraphicPipelineState(_key);
            m_graphicPipelineStateHash[_key] = pso;
        }

        _graphicPipelineState = pso;

        return nullptr != pso;
    }
}