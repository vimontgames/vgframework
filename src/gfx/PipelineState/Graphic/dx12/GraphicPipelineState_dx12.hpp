#if !VG_ENABLE_INLINE
#include "GraphicPipelineState_dx12.inl"
#endif

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    bool GraphicPipelineState::getd3d3d12Bytecode(HLSLDesc * _desc, ShaderStage _stage, ShaderKey::EntryPoint _entryPoint, ShaderKey::Flags _flags, D3D12_SHADER_BYTECODE * _d3d12ShaderBytecode)
    {
        if (ShaderKey::EntryPoint(-1) != _entryPoint)
        {
            const Shader * shader = _desc->getShader(API::DirectX12, _stage, _entryPoint, _flags);
            if (nullptr != shader)
            {
                *_d3d12ShaderBytecode = shader->getd3d12Bytecode();
                return true;
            }
        }
        else
        {
            *_d3d12ShaderBytecode = D3D12_SHADER_BYTECODE{ nullptr,0 };
            return true;
        }

        return false;
    }

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
            if (!getd3d3d12Bytecode(desc, ShaderStage::Vertex, _key.m_shaderKey.vs, _key.m_shaderKey.flags, &d3d12graphicPipelineDesc.VS))
                return false;

            if (!getd3d3d12Bytecode(desc, ShaderStage::Hull, _key.m_shaderKey.hs, _key.m_shaderKey.flags, &d3d12graphicPipelineDesc.HS))
                return false;
            
            if (!getd3d3d12Bytecode(desc, ShaderStage::Domain, _key.m_shaderKey.ds, _key.m_shaderKey.flags, &d3d12graphicPipelineDesc.DS))
                return false;
            
            if (!getd3d3d12Bytecode(desc, ShaderStage::Geometry, _key.m_shaderKey.gs, _key.m_shaderKey.flags, &d3d12graphicPipelineDesc.GS))
                return false;
            
            if (!getd3d3d12Bytecode(desc, ShaderStage::Pixel, _key.m_shaderKey.ps, _key.m_shaderKey.flags, &d3d12graphicPipelineDesc.PS))
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
                d3d12graphicPipelineDesc.RTVFormats[numRenderTargets] = Texture::getd3d12SurfaceFormat(format);
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
            d3d12graphicPipelineDesc.DSVFormat = Texture::getd3d12SurfaceFormat(format);
        }
        else
        {
            d3d12graphicPipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
        }

        d3d12graphicPipelineDesc.SampleDesc.Count = (uint)TextureDesc::getMSAASampleCount(_key.m_renderPassKey.m_msaa);
        d3d12graphicPipelineDesc.SampleDesc.Quality = 0;

        ID3D12PipelineState * d3d12GraphicPipelineState = nullptr;
        VG_VERIFY_SUCCEEDED(d3d12device->CreateGraphicsPipelineState(&d3d12graphicPipelineDesc, IID_PPV_ARGS(&d3d12GraphicPipelineState)));
        _d3d12GraphicPipelineState = d3d12GraphicPipelineState;

        return nullptr != d3d12GraphicPipelineState;
    }
}