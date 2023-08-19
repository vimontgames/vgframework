namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    VG_INLINE D3D12_PRIMITIVE_TOPOLOGY_TYPE GraphicPipelineState::getd3d12PrimitiveTopologyType(PrimitiveType _primitiveType)
    {
        switch (_primitiveType)
        {
            default:
                VG_ASSERT(false, "Unhandled PrimitiveType \"%s\" (%u)", core::asString(_primitiveType).c_str(), _primitiveType);

            case PrimitiveType::Point:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

            case PrimitiveType::Line:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

            case PrimitiveType::Triangle:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

                //case PrimitiveType::Patch:
                //    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        }
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void GraphicPipelineState::setd3d12GraphicPipelineState(ID3D12PipelineState * _d3d12GraphicPipelineState)
    {
        VG_ASSERT(nullptr == m_d3d12GraphicPipelineState);
        m_d3d12GraphicPipelineState = _d3d12GraphicPipelineState;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE ID3D12PipelineState * GraphicPipelineState::getd3d12GraphicPipelineState() const
    {
        return m_d3d12GraphicPipelineState;
    }
}