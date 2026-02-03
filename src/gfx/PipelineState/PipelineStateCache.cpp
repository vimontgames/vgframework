#include "gfx/Precomp.h"
#include "PipelineStateCache.h"
#include "core/Time/Timer.h"
#include "core/Scheduler/Mutex.h"
#include "gfx/Shader/HLSLDesc.h"
#include "gfx/Shader/ShaderManager.h"

using namespace vg::core;

#include "Graphic/GraphicPipelineStateCache.hpp"
#include "Compute/ComputePipelineStateCache.hpp"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    void PipelineStateCache::clear()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void PipelineStateCache::reset()
    {
        m_dirtyFlags = (GraphicPipelineStateCache::DirtyFlags)-1;
        memset(m_rootConstants, 0x0, sizeof(m_rootConstants));
    }
}