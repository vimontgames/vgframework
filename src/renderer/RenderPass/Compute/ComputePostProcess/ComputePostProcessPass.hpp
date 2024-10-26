#include "ComputePostProcessPass.h"
#include "Shaders/postprocess/postprocess.hlsli"
#include "Shaders/postprocess/postprocess.hlsl.h"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputePostProcessPass::ComputePostProcessPass() :
        ComputePass("ComputePostProcessPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, PostProcessConstantsCount);
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
    void ComputePostProcessPass::Setup(const RenderPassContext & _renderPassContext)
    {
        const auto colorID = _renderPassContext.getFrameGraphID("Color");
        readRenderTarget(colorID);

        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");
        readDepthStencil(depthStencilID);

        auto * device = Device::get();

        auto size = _renderPassContext.getView()->GetSize();

        FrameGraphTextureResourceDesc uavDesc;
        uavDesc.format = PixelFormat::R16G16B16A16_float;
        uavDesc.width = size.x;
        uavDesc.height = size.y;
        uavDesc.clearColor = defaultOptimizedClearColor;
        uavDesc.initState = FrameGraphResource::InitState::Clear;

        const auto dstID = _renderPassContext.getFrameGraphID("PostProcessUAV");
        createRWTexture(dstID, uavDesc);
        writeRWTexture(dstID);
    }

    //--------------------------------------------------------------------------------------
    void ComputePostProcessPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        auto size = _renderPassContext.getView()->GetSize();
        auto threadGroupSize = uint2(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y);
        auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.y + threadGroupSize.y - 1) / threadGroupSize.y, 1);

        ComputeShaderKey shaderKey = m_computePostProcessShaderKey;

        if (_renderPassContext.getView()->IsToolmode())
        {
            shaderKey.setFlag(PostProcessHLSLDesc::Toolmode, true);

            if (_renderPassContext.getView()->IsUsingRayTracing())
                shaderKey.setFlag(PostProcessHLSLDesc::RayTracing, true);
        }

        const auto * options = RendererOptions::get();
        const auto aaMode = options->GetAAPostProcess();
        switch (aaMode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(aaMode);

            case AAPostProcess::None:
                break;

            case AAPostProcess::FXAA:
                shaderKey.setFlag(PostProcessHLSLDesc::AAPostProcess, (uint)AAPostProcess::FXAA);
                break;

            case AAPostProcess::SMAA:
                shaderKey.setFlag(PostProcessHLSLDesc::AAPostProcess, (uint)AAPostProcess::SMAA);
                break;
        }

        _cmdList->setComputeRootSignature(m_computePostProcessRootSignature);
        _cmdList->setComputeShader(shaderKey);

        auto color = getRenderTarget(_renderPassContext.getFrameGraphID("Color"))->getTextureHandle();
        auto depthstencil = getDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        auto depth = depthstencil->getDepthTextureHandle();
        auto stencil = depthstencil->getStencilTextureHandle();
        auto dst = getRWTexture(_renderPassContext.getFrameGraphID("PostProcessUAV"))->getRWTextureHandle();
        
        PostProcessConstants postProcess;
        postProcess.width_height = packUint16(size.xy);
        postProcess.setColor(color);
        postProcess.setDepth(depth);
        postProcess.setStencil(stencil);
        postProcess.setRWBufferOut(dst);
        _cmdList->setComputeRootConstants(0, (u32*) &postProcess, PostProcessConstantsCount);

        _cmdList->dispatch(threadGroupCount);
    }
}