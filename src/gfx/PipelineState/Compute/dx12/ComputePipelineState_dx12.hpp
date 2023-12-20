#include "gfx/PipelineState/Graphic/GraphicPipelineState.h"

#if !VG_ENABLE_INLINE
#include "ComputePipelineState_dx12.inl"
#endif

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    ComputePipelineState::ComputePipelineState(const ComputePipelineStateKey & _computeKey) :
        super::ComputePipelineState(_computeKey)
    {

    }

    //--------------------------------------------------------------------------------------
    ComputePipelineState::~ComputePipelineState()
    {
        VG_SAFE_RELEASE(m_d3d12ComputePipelineState);
    }

    //--------------------------------------------------------------------------------------
    gfx::ComputePipelineState * ComputePipelineState::createComputePipelineState(const ComputePipelineStateKey & _computeKey)
    {
        ID3D12PipelineState * d3d12ComputePipelineState = nullptr;

        if (created3d12ComputePipelineState(_computeKey, d3d12ComputePipelineState))
        {
            VG_ASSERT(nullptr != d3d12ComputePipelineState);
            gfx::ComputePipelineState * pso = new gfx::ComputePipelineState(_computeKey);
            pso->setd3d12ComputePipelineState(d3d12ComputePipelineState);
            return pso;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool ComputePipelineState::created3d12ComputePipelineState(const ComputePipelineStateKey & _computeKey, ID3D12PipelineState *& _d3d12ComputePipelineState)
    {
        VG_ASSERT(ComputeShaderKey::CS(-1) != _computeKey.m_computeShaderKey.cs, "Cannot create ComputePipelineState because no Compute Shader is defined");

        auto * device = gfx::Device::get();
        auto * d3d12device = device->getd3d12Device();
        auto * sm = ShaderManager::get();

        D3D12_COMPUTE_PIPELINE_STATE_DESC d3d12ComputePipelineDesc = {};

        VG_ASSERT(_computeKey.m_computeRootSignature.isValid(), "Please set a valid Compute RootSignature using CommandList::setRootSignature()");
        auto * sig = device->getRootSignature(_computeKey.m_computeRootSignature);
        d3d12ComputePipelineDesc.pRootSignature = sig->getd3d12RootSignature();

        auto * desc = sm->getHLSLDescriptor(_computeKey.m_computeShaderKey.file);
        VG_ASSERT(desc);
        if (desc)
        {
            // TODO: move getd3d3d12Bytecode to some helper class instead?
            if (!GraphicPipelineState::getd3d3d12Bytecode(desc, ShaderStage::Compute, _computeKey.m_computeShaderKey.cs, _computeKey.m_computeShaderKey.flags, &d3d12ComputePipelineDesc.CS))
                return false;
        }

        ID3D12PipelineState * d3d12ComputePipelineState = nullptr;
        VG_ASSERT_SUCCEEDED(d3d12device->CreateComputePipelineState(&d3d12ComputePipelineDesc, IID_PPV_ARGS(&d3d12ComputePipelineState)));
        _d3d12ComputePipelineState = d3d12ComputePipelineState;

        return nullptr != d3d12ComputePipelineState;
    }
}