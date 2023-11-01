#include "ComputePostProcessPass.h"
#include "Shaders/postprocess/postprocess.hlsli"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputePostProcessPass::ComputePostProcessPass() :
        ComputePass("ComputePostProcessPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);

        m_computePostProcessRootSignature = device->addRootSignature(rsDesc);
        m_computePostProcessShaderKey.init("postprocess/postprocess.hlsl", "PostProcessMainCS");
    }

    //--------------------------------------------------------------------------------------
    ComputePostProcessPass::~ComputePostProcessPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_computePostProcessRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ComputePostProcessPass::Setup(const gfx::RenderPassContext & _renderPassContext, double _dt)
    {
        const auto srcID = _renderPassContext.getFrameGraphID("Color");
        readRenderTarget(srcID);

        auto * device = Device::get();

        auto size = _renderPassContext.m_view->GetSize();

        FrameGraph::TextureResourceDesc uavDesc;
        uavDesc.format = PixelFormat::R16G16B16A16_float;
        uavDesc.width = size.x;
        uavDesc.height = size.y;
        uavDesc.clearColor = float4(0, 0, 0, 0);
        uavDesc.initState = FrameGraph::Resource::InitState::Clear;

        const auto dstID = _renderPassContext.getFrameGraphID("PostProcessUAV");
        createRWTexture(dstID, uavDesc);
        writeRWTexture(0, dstID);
    }

    //--------------------------------------------------------------------------------------
    void ComputePostProcessPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        auto size = _renderPassContext.m_view->GetSize();
        auto threadGroupSize = uint2(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y);
        auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.x + threadGroupSize.y - 1) / threadGroupSize.y, 1);

        _cmdList->setComputeRootSignature(m_computePostProcessRootSignature);
        _cmdList->setComputeShader(m_computePostProcessShaderKey);

        u16 src = getRenderTarget(_renderPassContext.getFrameGraphID("Color"))->getBindlessSRVHandle();
        u16 dst = getRenderTarget(_renderPassContext.getFrameGraphID("PostProcessUAV"))->getBindlessUAVHandle();
        
        PostProcessConstants postProcess;
        postProcess.width_height = packUint16(size.xy);
        postProcess.srv_uav = packUint16(uint2(src, dst));
        _cmdList->setComputeRootConstants(0, (u32*) &postProcess, PostProcessConstantsCount);

        _cmdList->dispatch(threadGroupCount);
    }
}