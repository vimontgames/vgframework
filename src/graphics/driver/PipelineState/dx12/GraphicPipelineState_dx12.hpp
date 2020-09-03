namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    GraphicPipelineState::GraphicPipelineState() :
        super::GraphicPipelineState()
    {
        auto * d3d12Device = driver::Device::get()->getd3d12Device();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12graphicPipelineDesc = {};

        //d3d12graphicPipelineDesc.

        VG_ASSERT_SUCCEEDED(d3d12Device->CreateGraphicsPipelineState(&d3d12graphicPipelineDesc, IID_PPV_ARGS(&m_d3d12GraphicPipelineState)));
    }

    //--------------------------------------------------------------------------------------
    GraphicPipelineState::~GraphicPipelineState()
    {
  
    }
}