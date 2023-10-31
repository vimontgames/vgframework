namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setGraphicRootSignature(const RootSignatureHandle & _rsHandle)
    {
        super::setGraphicRootSignature(_rsHandle);
    }
                   
    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setRasterizerState(const gfx::RasterizerState & _rs) 
    {
        super::setRasterizerState(_rs);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setDepthStencilState(const gfx::DepthStencilState & _ds) 
    {
        super::setDepthStencilState(_ds);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setBlendState(const gfx::BlendState & _bs) 
    {
        super::setBlendState(_bs);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setShader(const ShaderKey & _key) 
    {
        super::setShader(_key);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setPrimitiveTopology(PrimitiveTopology _topology)
    {
        super::setPrimitiveTopology(_topology);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setViewport(const core::uint4 & _viewport) 
    {
        super::setViewport(_viewport);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setScissor(const core::uint4 & _scissor) 
    {
        super::setScissor(_scissor);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setGraphicRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
    {
        super::setGraphicRootConstants(_startOffset,_values, _count);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setIndexBuffer(gfx::Buffer * _ib) 
    {
        super::setIndexBuffer(_ib);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setComputeRootSignature(const RootSignatureHandle & _rsHandle)
    {
        super::setComputeRootSignature(_rsHandle);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setComputeShader(const ComputeShaderKey & _computeKey)
    {
        super::setComputeShader(_computeKey);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setComputeRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
    {
        super::setComputeRootConstants(_startOffset, _values, _count);
    }
}