#include "BackgroundPass.h"
#include "shaders/background/background.hlsli"
#include "renderer/Options/RendererOptions.h"
#include "gfx/IView.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    BackgroundPass::BackgroundPass() :
        Render2DPass("BackgroundPass")
    {
        auto * device = Device::get();

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();

        RootSignatureDesc rsDesc;
                          rsDesc.addRootConstants(ShaderStageFlags::VS | ShaderStageFlags::PS, 0, 0, sizeof(BackgroundRootConstants) / sizeof(u32));
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
    // Create main color and depth buffers and prepare for write
    //--------------------------------------------------------------------------------------
    void BackgroundPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        auto * device = Device::get();

        const auto size = _renderPassContext.getView()->GetSize();
        const auto options = RendererOptions::get();

        auto clearColor = m_useFastClear ? pow(options->getBackgroundColor(), 2.2f) : defaultOptimizedClearColor;

        FrameGraphTextureResourceDesc colorDesc = {};
                                      colorDesc.format = PixelFormat::R16G16B16A16_float;
                                      colorDesc.width = size.x;
                                      colorDesc.height = size.y;
                                      colorDesc.clearColor = clearColor;
                                      colorDesc.initState = FrameGraphResource::InitState::Clear;

        if (options->getLightingMode() == LightingMode::Deferred)
            colorDesc.uav = true;

        const auto colorID = _renderPassContext.getFrameGraphID("Color");
        createRenderTarget(colorID, colorDesc);
        writeRenderTarget(0, colorID);

        FrameGraphTextureResourceDesc depthStencilDesc = {};
                                      depthStencilDesc.format = PixelFormat::D32S8;
                                      depthStencilDesc.width = size.x;
                                      depthStencilDesc.height = size.y;
                                      depthStencilDesc.clearDepth = defaultOptimizedClearDepth;
                                      depthStencilDesc.clearStencil = defaultOptimizedClearStencil;
                                      depthStencilDesc.initState = FrameGraphResource::InitState::Clear;

        const auto depthStencilID = _renderPassContext.getFrameGraphID("DepthStencil");
        createDepthStencil(depthStencilID, depthStencilDesc);
        writeDepthStencil(depthStencilID);
    }

    //--------------------------------------------------------------------------------------
    void BackgroundPass::Render(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const
    {
        if (!m_useFastClear)
        {
            RasterizerState rs(FillMode::Solid, CullMode::None);
            BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);
            DepthStencilState ds(false);

            _cmdList->setGraphicRootSignature(m_backgroundRootSignatureHandle);
            _cmdList->setShader(m_backgroundShaderKey);
            _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
            _cmdList->setRasterizerState(rs);
            _cmdList->setBlendState(bs);
            _cmdList->setDepthStencilState(ds);

            BackgroundRootConstants root;
            root.reset();
            root.color = pow(RendererOptions::get()->getBackgroundColor(), 2.2f);

            _cmdList->setGraphicRootConstants(0, (u32 *)&root, sizeof(BackgroundRootConstants) / sizeof(u32));

            _cmdList->draw(4);
        }
    }
}