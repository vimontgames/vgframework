#if !VG_ENABLE_INLINE
#include "GraphicPipelineState_dx12.inl"
#endif

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    GraphicPipelineState::GraphicPipelineState(const GraphicPipelineStateKey & _key) :
        super::GraphicPipelineState(_key)
    {

    }

    //--------------------------------------------------------------------------------------
    GraphicPipelineState::~GraphicPipelineState()
    {
        VG_SAFE_RELEASE(m_d3d12GraphicPipelineState);
    }

    //--------------------------------------------------------------------------------------
    gfx::GraphicPipelineState * GraphicPipelineState::createGraphicPipelineState(const GraphicPipelineStateKey & _key)
    {
        ID3D12PipelineState * d3d12GraphicPipelineState = nullptr;
        
        if (created3d12GraphicPipelineState(_key, d3d12GraphicPipelineState))
        {
            VG_ASSERT(nullptr != d3d12GraphicPipelineState);
            gfx::GraphicPipelineState * pso = new gfx::GraphicPipelineState(_key);
            pso->setd3d12GraphicPipelineState(d3d12GraphicPipelineState);
            return pso;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool GraphicPipelineState::created3d12GraphicPipelineState(const GraphicPipelineStateKey & _key, ID3D12PipelineState *& _d3d12GraphicPipelineState)
    {
        VG_ASSERT(ShaderKey::VS(-1) != _key.m_shaderKey.vs, "Cannot create GraphicPipelineState because no Vertex Shader is defined");

        auto * device = gfx::Device::get();
        auto * d3d12device = device->getd3d12Device();
        auto * sm = ShaderManager::get();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12graphicPipelineDesc = {};

        VG_ASSERT(_key.m_rootSignature.isValid(), "Please set a valid RootSignature using CommandList::setRootSignature()");
        auto * sig = device->getRootSignature(_key.m_rootSignature);
        d3d12graphicPipelineDesc.pRootSignature = sig->getd3d12RootSignature();

        d3d12graphicPipelineDesc.BlendState = _key.m_blendState.getd3d12BlendState();

        d3d12graphicPipelineDesc.DepthStencilState = _key.m_depthStencilState.getd3d12DepthStencilState();

        d3d12graphicPipelineDesc.SampleMask = 0xFFFFFFFF;

        auto * desc = sm->getHLSLDescriptor(_key.m_shaderKey.file);
        VG_ASSERT(desc);
        if (desc)
        {
            GraphicsPrograms graphicsPrograms;
            
            if (!desc->getGraphicsPrograms(API::DirectX12, _key.m_shaderKey, graphicsPrograms))
                return false;

            if (graphicsPrograms.m_shaders[asInteger(GraphicsStage::Vertex)])
                d3d12graphicPipelineDesc.VS = graphicsPrograms.m_shaders[asInteger(GraphicsStage::Vertex  )]->getd3d12Bytecode();

            if (graphicsPrograms.m_shaders[asInteger(GraphicsStage::Hull)])
                d3d12graphicPipelineDesc.HS = graphicsPrograms.m_shaders[asInteger(GraphicsStage::Hull    )]->getd3d12Bytecode();

            if (graphicsPrograms.m_shaders[asInteger(GraphicsStage::Domain)])
                d3d12graphicPipelineDesc.DS = graphicsPrograms.m_shaders[asInteger(GraphicsStage::Domain  )]->getd3d12Bytecode();

            if (graphicsPrograms.m_shaders[asInteger(GraphicsStage::Geometry)])
                d3d12graphicPipelineDesc.GS = graphicsPrograms.m_shaders[asInteger(GraphicsStage::Geometry)]->getd3d12Bytecode();

            if (graphicsPrograms.m_shaders[asInteger(GraphicsStage::Pixel)])
                d3d12graphicPipelineDesc.PS = graphicsPrograms.m_shaders[asInteger(GraphicsStage::Pixel   )]->getd3d12Bytecode();
        }
        else
        {
            return false;
        }

        d3d12graphicPipelineDesc.RasterizerState = _key.m_rasterizerState.getd3d12RasterizerState();
        d3d12graphicPipelineDesc.PrimitiveTopologyType = getd3d12PrimitiveTopologyType(_key.getPrimitiveType());

        const auto subPassKey = _key.m_renderPassKey.m_subPassKeys[_key.m_subPassIndex];

        uint numRenderTargets = 0;
        for (uint i = 0; i < maxRenderTarget; ++i)
        {
            const ResourceTransitionDesc & info = subPassKey.getColorAttachmentInfo(i);
            if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
            {
                const auto format = _key.m_renderPassKey.m_colorFormat[i];
                VG_ASSERT(PixelFormat::Unknow != format);
                d3d12graphicPipelineDesc.RTVFormats[numRenderTargets] = Texture::getd3d12PixelFormat(format);
                ++numRenderTargets;
            }
            else
            {
                d3d12graphicPipelineDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
            }
        }
        d3d12graphicPipelineDesc.NumRenderTargets = numRenderTargets;

        const ResourceTransitionDesc info = subPassKey.getDepthStencilAttachmentInfo();
        if (asBool(ResourceTransitionFlags::RenderTarget & info.flags))
        {
            const auto format = _key.m_renderPassKey.m_depthStencilFormat;
            VG_ASSERT(PixelFormat::Unknow != format);
            d3d12graphicPipelineDesc.DSVFormat = Texture::getd3d12PixelFormat(format);
        }
        else
        {
            d3d12graphicPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
        }

        d3d12graphicPipelineDesc.SampleDesc.Count = 1;
        d3d12graphicPipelineDesc.SampleDesc.Quality = 0;

        ID3D12PipelineState * d3d12GraphicPipelineState = nullptr;
        VG_ASSERT_SUCCEEDED(d3d12device->CreateGraphicsPipelineState(&d3d12graphicPipelineDesc, IID_PPV_ARGS(&d3d12GraphicPipelineState)));
        _d3d12GraphicPipelineState = d3d12GraphicPipelineState;

        return nullptr != d3d12GraphicPipelineState;
    }
}