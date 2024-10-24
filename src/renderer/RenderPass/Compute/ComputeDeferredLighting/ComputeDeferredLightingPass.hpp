#include "ComputeDeferredLightingPass.h"
#include "Shaders/lighting/deferredLighting.hlsli"
#include "Shaders/lighting/deferredLighting.hlsl.h"

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
    void ComputeDeferredLightingPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto albedoGBufferID = _renderPassContext.getFrameGraphID("AlbedoGBuffer");
        readRenderTarget(albedoGBufferID);

        const auto normalGBufferID = _renderPassContext.getFrameGraphID("NormalGBuffer");
        readRenderTarget(normalGBufferID);

        const auto pbrGBufferID = _renderPassContext.getFrameGraphID("PBRGBuffer");
        readRenderTarget(pbrGBufferID);

        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");
        readDepthStencil(depthStencilID);

        const auto colorID = _renderPassContext.getFrameGraphID("Color");
        writeRWTexture(colorID);

        const View * view = static_cast<const View *>(_renderPassContext.getView());
        readDepthStencil(view->getShadowMaps());
    }

    //--------------------------------------------------------------------------------------
    void ComputeDeferredLightingPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {
        auto size = _renderPassContext.getView()->GetSize();
        auto threadGroupSize = uint2(DEFERRED_LIGHTING_THREADGROUP_SIZE_X, DEFERRED_LIGHTING_THREADGROUP_SIZE_Y);
        auto threadGroupCount = uint3((size.x + threadGroupSize.x - 1) / threadGroupSize.x, (size.y + threadGroupSize.y - 1) / threadGroupSize.y, 1);
        
        ComputeShaderKey shaderKey = m_computeDeferredLightingShaderKey;
        
        if (_renderPassContext.getView()->IsToolmode())
            shaderKey.setFlag(gfx::DeferredLightingHLSLDesc::Toolmode, true);

        if (_renderPassContext.getView()->IsUsingRayTracing())
            shaderKey.setFlag(gfx::DeferredLightingHLSLDesc::RayTracing, true);
        
        _cmdList->setComputeRootSignature(m_computeDeferredLightingRootSignature);
        _cmdList->setComputeShader(shaderKey);
        
        auto albedoID = getRenderTarget(_renderPassContext.getFrameGraphID("AlbedoGBuffer"))->getTextureHandle();
        auto normalID = getRenderTarget(_renderPassContext.getFrameGraphID("NormalGBuffer"))->getTextureHandle();
        auto pbrID = getRenderTarget(_renderPassContext.getFrameGraphID("PBRGBuffer"))->getTextureHandle();

        auto depthstencilTex = getDepthStencil(_renderPassContext.getFrameGraphID("DepthStencil"));
        auto depthID = depthstencilTex->getDepthTextureHandle();
        auto stencilID = depthstencilTex->getStencilTextureHandle();

        auto colorTex = getRWTexture(_renderPassContext.getFrameGraphID("Color"));
        auto colorID = colorTex->getRWTextureHandle();

        // Transition "Color" in UAV state for write
        _cmdList->transitionResource(colorTex, ResourceState::RenderTarget, ResourceState::UnorderedAccess);
        
        DeferredLightingConstants deferredLighting;
        deferredLighting.setScreenSize(size.xy);
        deferredLighting.setAlbedoGBuffer(albedoID);
        deferredLighting.setNormalGBuffer(normalID);
        deferredLighting.setPBRGBuffer(pbrID);
        deferredLighting.setDepth(depthID);
        deferredLighting.setStencil(stencilID);
        deferredLighting.setRWBufferOut(colorID);
        _cmdList->setComputeRootConstants(0, (u32 *)&deferredLighting, DeferredLightingConstantsCount);
        
        _cmdList->dispatch(threadGroupCount);

        _cmdList->addRWTextureBarrier(colorTex);

        // Transition "Color" back to RT state
        _cmdList->transitionResource(colorTex, ResourceState::UnorderedAccess, ResourceState::RenderTarget);
    }
}