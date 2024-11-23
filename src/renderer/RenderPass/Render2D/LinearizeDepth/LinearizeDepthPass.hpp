#include "LinearizeDepthPass.h"
#include "renderer/Options/RendererOptions.h"
#include "gfx/IView.h"

using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    LinearizeDepthPass::LinearizeDepthPass() :
        Render2DPass("LinearizeDepthPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, RootConstants2DCount);
        rsDesc.addTable(bindlessTable);

        m_linearizeDepthRootSignature = device->addRootSignature(rsDesc);

        m_linearizeDepthShaderKey.init("driver/driver.hlsl", "CopyLinearDepth");
        m_linearizeDepthMSAA2XShaderKey.init("driver/driver.hlsl", "ResolveLinearDepthMSAA2X");
        m_linearizeDepthMSAA4XShaderKey.init("driver/driver.hlsl", "ResolveLinearDepthMSAA4X");
        m_linearizeDepthMSAA8XShaderKey.init("driver/driver.hlsl", "ResolveLinearDepthMSAA8X");
        m_linearizeDepthMSAA16XShaderKey.init("driver/driver.hlsl", "ResolveLinearDepthMSAA16X");
    }

    //--------------------------------------------------------------------------------------
    LinearizeDepthPass::~LinearizeDepthPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_linearizeDepthRootSignature);
    }

    //--------------------------------------------------------------------------------------
    void LinearizeDepthPass::Setup(const RenderPassContext & _renderPassContext)
    {
        // Read depthstencil (it could be MSAA or non-MSAA)
        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");
        readDepthStencil(depthStencilID);

        // Create linear depth texture
        const FrameGraphTextureResourceDesc * depthStencilResourceDesc = getTextureResourceDesc(depthStencilID);

        FrameGraphTextureResourceDesc linearDepthResourceDesc = *depthStencilResourceDesc;
        linearDepthResourceDesc.type = TextureType::Texture2D;
        linearDepthResourceDesc.msaa = MSAA::None;
        linearDepthResourceDesc.format = PixelFormat::R16G16_float; // min/max depth

        const auto linearDepthID = _renderPassContext.getFrameGraphID("LinearDepth");
        createRenderTarget(linearDepthID, linearDepthResourceDesc);
        writeRenderTarget(0, linearDepthID);
    }

    //--------------------------------------------------------------------------------------
    void LinearizeDepthPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        const auto * options = RendererOptions::get();
        const auto msaa = options->GetMSAA();
        
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);
        
        _cmdList->setGraphicRootSignature(m_linearizeDepthRootSignature);
        
        switch (msaa)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(msaa);
        
            case MSAA::None:
                _cmdList->setShader(m_linearizeDepthShaderKey);
                break;

            case MSAA::MSAA2X:
                _cmdList->setShader(m_linearizeDepthMSAA2XShaderKey);
                break;
        
            case MSAA::MSAA4X:
                _cmdList->setShader(m_linearizeDepthMSAA4XShaderKey);
                break;
        
            case MSAA::MSAA8X:
                _cmdList->setShader(m_linearizeDepthMSAA8XShaderKey);
                break;
        
            case MSAA::MSAA16X:
                _cmdList->setShader(m_linearizeDepthMSAA16XShaderKey);
                break;
        }
        
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);
        
        RootConstants2D root2D;
        
        root2D.quad.posOffsetScale = float4(0, 0, 2, 2);
        root2D.quad.uvOffsetScale = float4(0, 0, 2, 2);
        root2D.texID = getRWTexture(_renderPassContext.getFrameGraphID("DepthStencil"))->getDepthTextureHandle();
        
        _cmdList->setGraphicRootConstants(0, (u32 *)&root2D, RootConstants2DCount);
        _cmdList->draw(4);
    }
}