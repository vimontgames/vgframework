namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    VG_INLINE ID3D12PipelineState * ComputePipelineState::getd3d12ComputePipelineState() const
    {
        return m_d3d12ComputePipelineState;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void ComputePipelineState::setd3d12ComputePipelineState(ID3D12PipelineState * _d3d12ComputePipelineState)
    {
        m_d3d12ComputePipelineState = _d3d12ComputePipelineState;
    }
}