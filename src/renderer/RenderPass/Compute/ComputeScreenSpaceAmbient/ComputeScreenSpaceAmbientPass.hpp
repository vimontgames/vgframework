#include "ComputeScreenSpaceAmbientPass.h"
#include "Shaders/lighting/screenSpaceAmbient.hlsli"
#include "Shaders/lighting/screenSpaceAmbient.hlsl.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputeScreenSpaceAmbientPass::ComputeScreenSpaceAmbientPass() :
        ComputePass("ComputeScreenSpaceAmbientPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, ScreenSpaceAmbientConstantsCount);
        rsDesc.addTable(bindlessTable);

        m_computeScreenSpaceAmbientRootSignature = device->addRootSignature(rsDesc);
        m_computeScreenSpaceAmbientShaderKey.init("lighting/screenSpaceAmbient.hlsl", "ScreenSpaceAmbient");
    }

    //--------------------------------------------------------------------------------------
    ComputeScreenSpaceAmbientPass::~ComputeScreenSpaceAmbientPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_computeScreenSpaceAmbientRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ComputeScreenSpaceAmbientPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto normalGBufferID = _renderPassContext.getFrameGraphID("NormalGBuffer");
        readRenderTarget(normalGBufferID);

        const auto linearDepthID = _renderPassContext.getFrameGraphID("LinearDepth");
        readRenderTarget(linearDepthID);

        const FrameGraphTextureResourceDesc * linearDepthDesc = getTextureResourceDesc(linearDepthID);

        const uint width = linearDepthDesc->width;
        const uint height = linearDepthDesc->height;

        FrameGraphTextureResourceDesc screenSpaceAmbientDesc = *linearDepthDesc;
        screenSpaceAmbientDesc.type = TextureType::Texture2D;
        screenSpaceAmbientDesc.width = width / 2;
        screenSpaceAmbientDesc.height = height / 2;
        screenSpaceAmbientDesc.msaa = MSAA::None;
        screenSpaceAmbientDesc.format = Renderer::get()->getLightingBufferFormat();

        const auto screenSpaceAmbientID = _renderPassContext.getFrameGraphID("ScreenSpaceAmbient");
        createRWTexture(screenSpaceAmbientID, screenSpaceAmbientDesc);
        writeRWTexture(screenSpaceAmbientID);
    }

    //--------------------------------------------------------------------------------------
    void ComputeScreenSpaceAmbientPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        const auto options = RendererOptions::get();
        const auto msaa = options->GetMSAA();
        const auto * view = (IView *)_renderPassContext.getView();
        auto size = view->GetSize() / 2;
        auto threadGroupSize = uint2(SCREEN_SPACE_AMBIENT_THREADGROUP_SIZE_X, SCREEN_SPACE_AMBIENT_THREADGROUP_SIZE_Y);
        auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.y + threadGroupSize.y - 1) / threadGroupSize.y, 1);

        ComputeShaderKey shaderKey = m_computeScreenSpaceAmbientShaderKey;

        if (view->IsToolmode())
            shaderKey.setFlag(ScreenSpaceAmbientHLSLDesc::Flags::Toolmode, true);

        //if (view->IsUsingRayTracing())
        //    shaderKey.setFlag(ScreenSpaceAmbientHLSLDesc::Flags::RayTracing, true);

        _cmdList->setComputeRootSignature(m_computeScreenSpaceAmbientRootSignature);
        _cmdList->setComputeShader(shaderKey);

        const auto linearDepth = getRWTexture(_renderPassContext.getFrameGraphID("LinearDepth"))->getTextureHandle();;
        const auto normal = getRenderTarget(_renderPassContext.getFrameGraphID("NormalGBuffer"))->getTextureHandle();

        Texture * dstTex = getRWTexture(_renderPassContext.getFrameGraphID("ScreenSpaceAmbient"));
        
        ScreenSpaceAmbientConstants screenSpaceAmbientConstants;
        screenSpaceAmbientConstants.setScreenSize(size.xy);
        screenSpaceAmbientConstants.setLinearDepthBuffer(linearDepth);
        screenSpaceAmbientConstants.setNormalGBuffer(normal);
        screenSpaceAmbientConstants.setRWBufferOut(dstTex->getRWTextureHandle());

        _cmdList->setComputeRootConstants(0, (u32 *)&screenSpaceAmbientConstants, ScreenSpaceAmbientConstantsCount);

        _cmdList->dispatch(threadGroupCount);

        _cmdList->addRWTextureBarrier(dstTex);
    }
}