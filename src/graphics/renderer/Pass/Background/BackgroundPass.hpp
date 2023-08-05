#include "BackgroundPass.h"
#include "shaders/background/background.hlsli"
#include "graphics/renderer/Options/DisplayOptions.h"
#include "gfx/IView.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    BackgroundPass::BackgroundPass() :
        gfx::UserPass("BackgroundPass")
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
    void BackgroundPass::setup(const gfx::RenderContext & _renderContext, double _dt)
    {
        auto * device = Device::get();

        auto size = _renderContext.m_view->GetSize();

        FrameGraph::TextureResourceDesc colorDesc;
                                        colorDesc.format = PixelFormat::R16G16B16A16_float;
                                        colorDesc.width = size.x;
                                        colorDesc.height = size.y;
                                        colorDesc.clearColor = float4(0, 0, 0, 0);
                                        colorDesc.initState = FrameGraph::Resource::InitState::Clear;

        createRenderTarget(_renderContext.getName("Color"), colorDesc);
        writeRenderTarget(0, _renderContext.getName("Color"));

        FrameGraph::TextureResourceDesc depthStencilDesc;
                                        depthStencilDesc.format = PixelFormat::D32S8;
                                        depthStencilDesc.width = size.x;
                                        depthStencilDesc.height = size.y;
                                        depthStencilDesc.clearDepth = 1.0f;
                                        depthStencilDesc.clearStencil = 0x0;
                                        depthStencilDesc.initState = FrameGraph::Resource::InitState::Clear;

        createRenderTarget(_renderContext.getName("DepthStencil"), depthStencilDesc);
        writeDepthStencil(_renderContext.getName("DepthStencil"));
    }

    //--------------------------------------------------------------------------------------
    void BackgroundPass::draw(const RenderContext & _renderContext, CommandList * _cmdList) const
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
        root.color = pow(DisplayOptions::get()->getBackgroundColor(), 2.2f);
        
        _cmdList->setInlineRootConstants(&root, sizeof(BackgroundRootConstants)/sizeof(u32));
        
        _cmdList->draw(4);
    }
}