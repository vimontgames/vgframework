#include "gfx/Precomp.h"
#include "gfx/Device/Device.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/Shader/Shader.h"
#include "gfx/Shader/HLSLDesc.h"
#include "gfx/RootSignature/RootSignature.h"
#include "gfx/Resource/Texture.h"
#include "ComputePipelineState.h"

#include "ComputePipelineStateKey.hpp"

#ifdef VG_DX12
#include "dx12/ComputePipelineState_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/ComputePipelineState_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

using namespace vg::core;

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        ComputePipelineState::ComputePipelineState(const ComputePipelineStateKey & _computeKey) :
            m_computeKey(_computeKey)
        {

        }

        //--------------------------------------------------------------------------------------
        ComputePipelineState::~ComputePipelineState()
        {

        }

    }
}

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    ComputePipelineState::ComputePipelineState(const ComputePipelineStateKey & _computeKey) :
        super(_computeKey)
    {

    }

    //--------------------------------------------------------------------------------------
    ComputePipelineState::~ComputePipelineState()
    {

    }

    //--------------------------------------------------------------------------------------
    ComputePipelineState * ComputePipelineState::createComputePipelineState(const ComputePipelineStateKey & _computeKey)
    {
        VG_PROFILE_CPU("CreateComputePSO");
        return super::createComputePipelineState(_computeKey);
    }
}