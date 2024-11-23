#include "ComputeDeferredLightingPass.h"
#include "Shaders/lighting/deferredLighting.hlsli"
#include "Shaders/lighting/deferredLighting.hlsl.h"
#include "Shaders/system/msaa.hlsli"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    ComputeDeferredLightingPass::ComputeDeferredLightingPass() :
        ComputePass("ComputeDeferredLightingPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::CS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);

        m_computeDeferredLightingRootSignature = device->addRootSignature(rsDesc);
        m_computeDeferredLightingShaderKey.init("lighting/deferredLighting.hlsl", "DeferredLightingCS");
    }

    //--------------------------------------------------------------------------------------
    ComputeDeferredLightingPass::~ComputeDeferredLightingPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_computeDeferredLightingRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void ComputeDeferredLightingPass::Setup(const RenderPassContext & _renderPassContext)
    {
        const auto albedoGBufferID = _renderPassContext.getFrameGraphID("AlbedoGBuffer");
        readRenderTarget(albedoGBufferID);

        const auto normalGBufferID = _renderPassContext.getFrameGraphID("NormalGBuffer");
        readRenderTarget(normalGBufferID);

        const auto pbrGBufferID = _renderPassContext.getFrameGraphID("PBRGBuffer");
        readRenderTarget(pbrGBufferID);

        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");
        readDepthStencil(depthStencilID);

        const View * view = static_cast<const View *>(_renderPassContext.getView());
        readDepthStencil(view->getShadowMaps());

        const RendererOptions * options = RendererOptions::get();
        const MSAA msaa = options->GetMSAA();
        if (MSAA::None != msaa)
        {
            const auto colorID = _renderPassContext.getFrameGraphID("Color");
            const FrameGraphTextureResourceDesc * colorResourceDesc = getTextureResourceDesc(colorID);

            const int2 msaaScale = getMSAASampleScale((uint)TextureDesc::getMSAASampleCount(msaa));

            const uint width = colorResourceDesc->width * msaaScale.x;
            const uint height = colorResourceDesc->height * msaaScale.y;

            FrameGraphTextureResourceDesc resolvedDesc = *colorResourceDesc;
                                          resolvedDesc.type = TextureType::Texture2D;
                                          resolvedDesc.width = width;
                                          resolvedDesc.height = height;
                                          resolvedDesc.msaa = MSAA::None;

            const auto resolveID = _renderPassContext.getFrameGraphID("ResolveDeferredMSAA");
            createRWTexture(resolveID, resolvedDesc);
            writeRWTexture(resolveID);
        }
        else
        {
            const auto colorID = _renderPassContext.getFrameGraphID("Color");
            writeRWTexture(colorID);
        }
    }

    //--------------------------------------------------------------------------------------
    void ComputeDeferredLightingPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const auto options = RendererOptions::get();
        const auto msaa = options->GetMSAA();

        auto size = _renderPassContext.getView()->GetSize();
        auto threadGroupSize = uint2(DEFERRED_LIGHTING_THREADGROUP_SIZE_X, DEFERRED_LIGHTING_THREADGROUP_SIZE_Y);
        auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.y + threadGroupSize.y - 1) / threadGroupSize.y, 1);
        
        ComputeShaderKey shaderKey = m_computeDeferredLightingShaderKey;
        
        if (_renderPassContext.getView()->IsToolmode())
            shaderKey.setFlag(DeferredLightingHLSLDesc::Flags::Toolmode, true);

        if (_renderPassContext.getView()->IsUsingRayTracing())
            shaderKey.setFlag(DeferredLightingHLSLDesc::Flags::RayTracing, true);

        switch (msaa)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(msaa);

            case MSAA::None:
                break;

            case MSAA::MSAA2X:
                shaderKey.setValue(DeferredLightingHLSLDesc::Flags::MSAA, MSAA::MSAA2X);
                break;

            case MSAA::MSAA4X:
                shaderKey.setValue(DeferredLightingHLSLDesc::Flags::MSAA, MSAA::MSAA4X);
                break;

            case MSAA::MSAA8X:
                shaderKey.setValue(DeferredLightingHLSLDesc::Flags::MSAA, MSAA::MSAA8X);
                break;

            case MSAA::MSAA16X:
                shaderKey.setValue(DeferredLightingHLSLDesc::Flags::MSAA, MSAA::MSAA16X);
                break;
        }
        
        _cmdList->setComputeRootSignature(m_computeDeferredLightingRootSignature);
        _cmdList->setComputeShader(shaderKey);
        
        auto albedoID = getRenderTarget(_renderPassContext.getFrameGraphID("AlbedoGBuffer"))->getTextureHandle();
        auto normalID = getRenderTarget(_renderPassContext.getFrameGraphID("NormalGBuffer"))->getTextureHandle();
        auto pbrID = getRenderTarget(_renderPassContext.getFrameGraphID("PBRGBuffer"))->getTextureHandle();

        auto depthstencilTex = getDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        auto depthID = depthstencilTex->getDepthTextureHandle();
        auto stencilID = depthstencilTex->getStencilTextureHandle();

        Texture * dstTex = nullptr;
        BindlessRWTextureHandle dstHandle;

        if (msaa == MSAA::None)
        {
            // main color buffer is UAV we can directly write to it
            dstTex = getRWTexture(_renderPassContext.getFrameGraphID("Color"));

            // As it's also used as RT, we need to transition to UAV state for write
            _cmdList->transitionResource(dstTex, ResourceState::RenderTarget, ResourceState::UnorderedAccess);
        }
        else
        {
            // as we cant render to MSAA RT from CS, we need to copy to an intermediate non-MSAA "Resolve" target
            const auto resolveID = _renderPassContext.getFrameGraphID("ResolveDeferredMSAA");
            dstTex = getRWTexture(resolveID);
        }
        
        DeferredLightingConstants deferredLighting;
        deferredLighting.setScreenSize(size.xy);
        deferredLighting.setAlbedoGBuffer(albedoID);
        deferredLighting.setNormalGBuffer(normalID);
        deferredLighting.setPBRGBuffer(pbrID);
        deferredLighting.setDepth(depthID);
        deferredLighting.setStencil(stencilID);
        deferredLighting.setRWBufferOut(dstTex->getRWTextureHandle());
        _cmdList->setComputeRootConstants(0, (u32 *)&deferredLighting, DeferredLightingConstantsCount);
        
        _cmdList->dispatch(threadGroupCount);

        _cmdList->addRWTextureBarrier(dstTex);

        if (msaa == MSAA::None)
        {
            // Destination UAV will be used as RT from now
            _cmdList->transitionResource(dstTex, ResourceState::UnorderedAccess, ResourceState::RenderTarget);
        }
    }
}