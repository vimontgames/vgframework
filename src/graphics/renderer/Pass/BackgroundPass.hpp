#include "BackgroundPass.h"
#include "shaders/background/background.hlsli"

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

        FrameGraph::TextureResourceDesc desc;
        desc.format = device->getBackbufferFormat();
        desc.width = deviceParams.resolution.x >> 1;
        desc.height = deviceParams.resolution.y >> 1;
        desc.clearColor = float4(0, 0, 0, 0);
        desc.initState = FrameGraph::Resource::InitState::Clear;

        createRenderTarget("Color", desc);
        writeRenderTarget(0, "Color");
    }

    //--------------------------------------------------------------------------------------
    void BackgroundPass::draw(CommandList * _cmdList) const
    {
        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        _cmdList->setRootSignature(m_backgroundRootSignatureHandle);
        _cmdList->setShader(m_backgroundShaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);

        BackgroundRootConstants root;

        root.quad.posOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        root.quad.uvOffsetScale = float4(0.0f, 0.0f, 1.0f, 1.0f);
        
        _cmdList->setInlineRootConstants(&root, sizeof(BackgroundRootConstants)/sizeof(u32));
        
        _cmdList->draw(4);
    }
}