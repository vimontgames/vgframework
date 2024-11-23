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
        m_computeResolveMSAAShaderKey.init("postprocess/postprocess.hlsl", "ResolveMSAACS");
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

        const auto linearDepthID = _renderPassContext.getFrameGraphID("LinearDepth");
        readRenderTarget(linearDepthID);

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

        const auto * options = RendererOptions::get();
        const auto msaa = options->GetMSAA();
        if (msaa != MSAA::None)
        {
            // temp target for resolved MSAA color buffer
            const auto dstID = _renderPassContext.getFrameGraphID("ResolveColorUAV");
            createRWTexture(dstID, uavDesc);
            writeRWTexture(dstID);
        }
    }

    //--------------------------------------------------------------------------------------
    void ComputePostProcessPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const auto * options = RendererOptions::get();

        auto size = _renderPassContext.getView()->GetSize();
        auto threadGroupSize = uint2(POSTPROCESS_THREADGROUP_SIZE_X, POSTPROCESS_THREADGROUP_SIZE_Y);
        auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.y + threadGroupSize.y - 1) / threadGroupSize.y, 1);

        const auto msaa = options->GetMSAA();
        Texture * resolveUAVTex = nullptr;
        if (msaa != MSAA::None)
        {
            VG_PROFILE_GPU("ResolveMSAA");

            ComputeShaderKey resolveShaderKey = m_computeResolveMSAAShaderKey;

            if (_renderPassContext.getView()->IsToolmode())
                resolveShaderKey.setFlag(PostProcessHLSLDesc::Toolmode, true);

            // When MSAA is enabled, we will first resolve the MSAA color buffer to a non-MSAA UAV then use it as input for the next pass
            const auto msaa = options->GetMSAA();
            if (msaa != MSAA::None)
            {
                switch (msaa)
                {
                    default:
                        VG_ASSERT_ENUM_NOT_IMPLEMENTED(msaa);
                        break;
            
                    case MSAA::MSAA2X:
                        resolveShaderKey.setValue(PostProcessHLSLDesc::MSAA, MSAA::MSAA2X);
                        break;
            
                    case MSAA::MSAA4X:
                        resolveShaderKey.setValue(PostProcessHLSLDesc::MSAA, MSAA::MSAA4X);
                        break;
            
                    case MSAA::MSAA8X:
                        resolveShaderKey.setValue(PostProcessHLSLDesc::MSAA, MSAA::MSAA8X);
                        break;
            
                    case MSAA::MSAA16X:
                        resolveShaderKey.setValue(PostProcessHLSLDesc::MSAA, MSAA::MSAA16X);
                        break;
                }
            }

            _cmdList->setComputeRootSignature(m_computePostProcessRootSignature);
            _cmdList->setComputeShader(resolveShaderKey);

            auto colorTex = getRenderTarget(_renderPassContext.getFrameGraphID("Color"));
            auto color = colorTex->getTextureHandle(); 
            
            resolveUAVTex = getRWTexture(_renderPassContext.getFrameGraphID("ResolveColorUAV"));
            auto dest = resolveUAVTex->getRWTextureHandle();

            PostProcessConstants postProcess;
            postProcess.setScreenSize(size.xy);
            postProcess.setColor(color);
            postProcess.setRWBufferOut(dest);

            _cmdList->setComputeRootConstants(0, (u32 *)&postProcess, PostProcessConstantsCount);

            _cmdList->dispatch(threadGroupCount);

            _cmdList->addRWTextureBarrier(resolveUAVTex);
            _cmdList->transitionResource(resolveUAVTex, ResourceState::UnorderedAccess, ResourceState::ShaderResource);
        }       

        ComputeShaderKey shaderKey = m_computePostProcessShaderKey;

        if (_renderPassContext.getView()->IsToolmode())
        {
            shaderKey.setFlag(PostProcessHLSLDesc::Toolmode, true);

            if (_renderPassContext.getView()->IsUsingRayTracing())
                shaderKey.setFlag(PostProcessHLSLDesc::RayTracing, true);
        }

        const auto aaMode = options->GetAAPostProcess();
        switch (aaMode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(aaMode);

            case AAPostProcess::None:
                break;

            case AAPostProcess::FXAA:
                shaderKey.setValue(PostProcessHLSLDesc::AAPostProcess, AAPostProcess::FXAA);
                break;

            case AAPostProcess::SMAA:
                shaderKey.setValue(PostProcessHLSLDesc::AAPostProcess, AAPostProcess::SMAA);
                break;
        }

        _cmdList->setComputeRootSignature(m_computePostProcessRootSignature);
        _cmdList->setComputeShader(shaderKey);

        auto srcID = msaa == MSAA::None ? _renderPassContext.getFrameGraphID("Color") : _renderPassContext.getFrameGraphID("ResolveColorUAV");
        auto color = getRenderTarget(srcID)->getTextureHandle();

        auto depthstencilTex = getDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        auto depth = depthstencilTex->getDepthTextureHandle();
        auto stencil = depthstencilTex->getStencilTextureHandle();

        auto linearDepthTex = getRenderTarget(_renderPassContext.getFrameGraphID("LinearDepth"));
        auto linearDepth = linearDepthTex->getTextureHandle();
 
        auto dest = getRWTexture(_renderPassContext.getFrameGraphID("PostProcessUAV"))->getRWTextureHandle();
        
        PostProcessConstants postProcess;
        postProcess.setScreenSize(size.xy);
        postProcess.setColor(color);
        postProcess.setRWBufferOut(dest);
        postProcess.setDepth(depth);
        postProcess.setStencil(stencil);     
        postProcess.setLinearDepth(linearDepth);

        _cmdList->setComputeRootConstants(0, (u32*) &postProcess, PostProcessConstantsCount);

        _cmdList->dispatch(threadGroupCount);

        if (MSAA::None != msaa)
            _cmdList->transitionResource(resolveUAVTex, ResourceState::ShaderResource, ResourceState::UnorderedAccess);
    }
}