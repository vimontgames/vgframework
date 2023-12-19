#include "gfx/Precomp.h"
#include "gfx/Device/Device.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/Shader/Shader.h"
#include "gfx/Shader/HLSLDesc.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/Resource/Texture.h"
#include "RayTracingPipelineState.h"

using namespace vg::core;

#include "RayTracingPipelineStateKey.hpp"

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        RayTracingPipelineState::RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey) :
            m_rayTracingKey(_rayTracingKey)
        {

        }

        //--------------------------------------------------------------------------------------
        RayTracingPipelineState::~RayTracingPipelineState()
        {

        }

    }
}

#include VG_GFXAPI_IMPL(RayTracingPipelineState) 

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    RayTracingPipelineState::RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey) :
        super(_rayTracingKey)
    {

    }

    //--------------------------------------------------------------------------------------
    RayTracingPipelineState::~RayTracingPipelineState()
    {

    }
}
