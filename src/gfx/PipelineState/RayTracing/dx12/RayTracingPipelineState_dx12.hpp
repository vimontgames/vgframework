#include "gfx/PipelineState/RayTracing/RayTracingPipelineState.h"
#include "gfx/PipelineState/Graphic/GraphicPipelineState.h"
#include "core/string/string.h"

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    RayTracingPipelineState::RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey) :
        super(_rayTracingKey)
    {

    }

    //--------------------------------------------------------------------------------------
    RayTracingPipelineState::~RayTracingPipelineState()
    {
        VG_SAFE_RELEASE(m_d3d12RayTracingPipelineState);
    }

    //--------------------------------------------------------------------------------------
    ID3D12StateObject * RayTracingPipelineState::getd3d12RayTracingPipelineState() const
    {
        return m_d3d12RayTracingPipelineState;
    }

    //--------------------------------------------------------------------------------------
    void RayTracingPipelineState::setd3d12RayTracingPipelineState(ID3D12StateObject * _d3d12RayTracingPipelineState)
    {
        m_d3d12RayTracingPipelineState = _d3d12RayTracingPipelineState;
    }

    //--------------------------------------------------------------------------------------
    gfx::RayTracingPipelineState * RayTracingPipelineState::createRayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey)
    {
        ID3D12StateObject * d3d12RayTracingPipelineState = nullptr;
        
        if (created3d12RayTracingPipelineState(_rayTracingKey, d3d12RayTracingPipelineState))
        {
            VG_ASSERT(nullptr != d3d12RayTracingPipelineState);
            gfx::RayTracingPipelineState * pso = new gfx::RayTracingPipelineState(_rayTracingKey);
            pso->setd3d12RayTracingPipelineState(d3d12RayTracingPipelineState);
            return pso;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool RayTracingPipelineState::created3d12RayTracingPipelineState(const RayTracingPipelineStateKey & _rayTracingKey, ID3D12StateObject *& _d3d12RayTracingPipelineState)
    {
        //VG_ASSERT(RayTracingShaderKey::RG(-1) != _rayTracingKey.m_rayTracingShaderKey.rg, "Cannot create RayTracingPipelineState because no RayGen Shader is defined");
        //
        //auto * device = gfx::Device::get();
        //auto * d3d12device = device->getd3d12Device();
        //auto * sm = ShaderManager::get();
        //
        //CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };
        //
        //VG_ASSERT(_rayTracingKey.m_rayTracingRootSignature.isValid(), "Please set a valid RootSignature using CommandList::setRayTracingRootSignature()");
        //auto * sig = device->getRootSignature(_rayTracingKey.m_rayTracingRootSignature);
        ////d3d12graphicPipelineDesc.pRootSignature = sig->getd3d12RootSignature();
        //
        //HLSLDesc * desc = sm->getHLSLDescriptor(_rayTracingKey.m_rayTracingShaderKey.file);
        //VG_ASSERT(desc);
        //
        //vector<D3D12_DXIL_LIBRARY_DESC> libraries;
        //libraries.reserve(5);
        //
        //auto addRayTracingStageLib = [](vector<D3D12_DXIL_LIBRARY_DESC> & _libraries, HLSLDesc * _desc, ShaderStage _stage, ShaderKey::EntryPoint _entryPoint, ShaderKey::Flags _flags)
        //{
        //    D3D12_SHADER_BYTECODE bytecode;
        //
        //    if (ShaderKey::EntryPoint(-1) != _entryPoint)
        //    {
        //        if (GraphicPipelineState::getd3d3d12Bytecode(_desc, _stage, _entryPoint, _flags, &bytecode))
        //        {
        //            D3D12_DXIL_LIBRARY_DESC & lib = _libraries.push_empty();
        //            lib.DXILLibrary.pShaderBytecode = bytecode.pShaderBytecode;
        //            lib.DXILLibrary.BytecodeLength = bytecode.BytecodeLength;
        //
        //            auto pExports = new D3D12_EXPORT_DESC();
        //            pExports->Name = _wcsdup(wstring_convert(_desc->getEntryPoint(_stage, _entryPoint)).c_str());
        //            pExports->Flags = D3D12_EXPORT_FLAG_NONE;
        //
        //            lib.pExports = pExports;
        //            lib.NumExports = 1;
        //
        //            return true;
        //        }
        //        else
        //        {
        //            return false;
        //        }
        //    }
        //    else
        //    {
        //        return true;
        //    }            
        //};
        //
        //if (desc)
        //{
        //    if (!addRayTracingStageLib(libraries, desc, ShaderStage::RayGen, _rayTracingKey.m_rayTracingShaderKey.rg, _rayTracingKey.m_rayTracingShaderKey.flags))
        //        return false;
        //
        //    if (!addRayTracingStageLib(libraries, desc, ShaderStage::AnyHit, _rayTracingKey.m_rayTracingShaderKey.ah, _rayTracingKey.m_rayTracingShaderKey.flags))
        //        return false;
        //
        //    if (!addRayTracingStageLib(libraries, desc, ShaderStage::ClosestHit, _rayTracingKey.m_rayTracingShaderKey.ch, _rayTracingKey.m_rayTracingShaderKey.flags))
        //        return false;
        //
        //    if (!addRayTracingStageLib(libraries, desc, ShaderStage::Miss, _rayTracingKey.m_rayTracingShaderKey.mi, _rayTracingKey.m_rayTracingShaderKey.flags))
        //        return false;
        //
        //    if (!addRayTracingStageLib(libraries, desc, ShaderStage::Intersection, _rayTracingKey.m_rayTracingShaderKey.is, _rayTracingKey.m_rayTracingShaderKey.flags))
        //        return false;
        //}

        //VG_ASSERT(ComputeShaderKey::CS(-1) != _computeKey.m_computeShaderKey.cs, "Cannot create ComputePipelineState because no Compute Shader is defined");
        //
        //auto * device = gfx::Device::get();
        //auto * d3d12device = device->getd3d12Device();
        //auto * sm = ShaderManager::get();
        //
        //D3D12_COMPUTE_PIPELINE_STATE_DESC d3d12ComputePipelineDesc = {};
        //
        //VG_ASSERT(_computeKey.m_computeRootSignature.isValid(), "Please set a valid Compute RootSignature using CommandList::setRootSignature()");
        //auto * sig = device->getRootSignature(_computeKey.m_computeRootSignature);
        //d3d12ComputePipelineDesc.pRootSignature = sig->getd3d12RootSignature();
        //
        //auto * desc = sm->getHLSLDescriptor(_computeKey.m_computeShaderKey.file);
        //VG_ASSERT(desc);
        //if (desc)
        //{
        //    // TODO: move getd3d3d12Bytecode to some helper class instead?
        //    if (!GraphicPipelineState::getd3d3d12Bytecode(desc, ShaderStage::Compute, _computeKey.m_computeShaderKey.cs, _computeKey.m_computeShaderKey.flags, &d3d12ComputePipelineDesc.CS))
        //        return false;
        //}
        //
        //ID3D12PipelineState * d3d12ComputePipelineState = nullptr;
        //VG_ASSERT_SUCCEEDED(d3d12device->CreateComputePipelineState(&d3d12ComputePipelineDesc, IID_PPV_ARGS(&d3d12ComputePipelineState)));
        //_d3d12ComputePipelineState = d3d12ComputePipelineState;
    
        return nullptr != _d3d12RayTracingPipelineState;
    }
}