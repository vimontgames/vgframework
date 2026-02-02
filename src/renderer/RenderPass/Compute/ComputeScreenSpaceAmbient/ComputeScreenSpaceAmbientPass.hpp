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

    }

    //--------------------------------------------------------------------------------------
    ScreenSpaceAmbient ComputeScreenSpaceAmbientPass::getScreenSpaceAmbient(const IView * _view) const
    {
        const auto * camSettings = _view->GetCameraSettings();
        if (nullptr != camSettings)
            return camSettings->GetScreenSpaceAmbient();
        return ScreenSpaceAmbient::None;
    }

    //--------------------------------------------------------------------------------------
    void ComputeScreenSpaceAmbientPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        const auto normalGBufferID = _renderPassContext.getFrameGraphID("NormalGBuffer");
        readRenderTarget(normalGBufferID);

        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");
        readDepthStencil(depthStencilID);

        const FrameGraphTextureResourceDesc * depthStencilDesc = getTextureResourceDesc(depthStencilID);

        const uint width = depthStencilDesc->width;
        const uint height = depthStencilDesc->height;

        FrameGraphTextureResourceDesc screenSpaceAmbientDesc = *depthStencilDesc;
        screenSpaceAmbientDesc.type = TextureType::Texture2D;
        screenSpaceAmbientDesc.width = width;
        screenSpaceAmbientDesc.height = height;
        screenSpaceAmbientDesc.msaa = MSAA::None;
        screenSpaceAmbientDesc.format = Renderer::get()->getLightingBufferFormat();

        const auto screenSpaceAmbientID = _renderPassContext.getFrameGraphID("ScreenSpaceAmbient");
        createRWTexture(screenSpaceAmbientID, screenSpaceAmbientDesc);
        writeRWTexture(screenSpaceAmbientID);
    }

    //--------------------------------------------------------------------------------------
    void ComputeScreenSpaceAmbientPass::Render(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList) const
    {

    }
}