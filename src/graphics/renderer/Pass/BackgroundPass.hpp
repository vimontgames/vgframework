#include "BackgroundPass.h"
#include "shaders/background/background.hlsli"
#include "graphics/renderer/Options/DisplayOptions.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    BackgroundPass::BackgroundPass()
    {
        auto * device = Device::get();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, sizeof(BackgroundRootConstants) / sizeof(u32));

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);

        m_backgroundRootSignatureHandle = device->addRootSignature(rsDesc);

        m_backgroundShaderKey.init("background/background.hlsl", "Gradient");
    }

    //--------------------------------------------------------------------------------------
    BackgroundPass::~BackgroundPass()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_backgroundRootSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void BackgroundPass::setup(double _dt)
    {
        auto * device = Device::get();

        const driver::DeviceParams & deviceParams = device->getDeviceParams();

        FrameGraph::TextureResourceDesc colorDesc;
                                        colorDesc.format = PixelFormat::R16G16B16A16_float;
                                        colorDesc.width = deviceParams.resolution.x;
                                        colorDesc.height = deviceParams.resolution.y;
                                        colorDesc.clearColor = float4(0, 0, 0, 0);
                                        colorDesc.initState = FrameGraph::Resource::InitState::Clear;

        createRenderTarget("Color", colorDesc);
        writeRenderTarget(0, "Color");

        FrameGraph::TextureResourceDesc depthStencilDesc;
                                        depthStencilDesc.format = PixelFormat::D32S8;
                                        depthStencilDesc.width = deviceParams.resolution.x;
                                        depthStencilDesc.height = deviceParams.resolution.y;
                                        depthStencilDesc.clearDepth = 1.0f;
                                        depthStencilDesc.clearStencil = 0x0;
                                        depthStencilDesc.initState = FrameGraph::Resource::InitState::Clear;

        createRenderTarget("DepthStencil", depthStencilDesc);
        writeDepthStencil("DepthStencil");
    }

    //--------------------------------------------------------------------------------------
    void BackgroundPass::draw(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
        DepthStencilState ds(false);

        _cmdList->setRootSignature(m_backgroundRootSignatureHandle);
        _cmdList->setShader(m_backgroundShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);
        _cmdList->setDepthStencilState(ds);

        BackgroundRootConstants root;

        root.quad.posOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root.color = DisplayOptions::get()->getBackgroundColor();
        
        _cmdList->setInlineRootConstants(&root, sizeof(BackgroundRootConstants)/sizeof(u32));
        
        _cmdList->draw(4);
    }
}