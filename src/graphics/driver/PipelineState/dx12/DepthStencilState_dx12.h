#pragma once

#include "graphics/driver/PipelineState/DepthStencilState.h"

namespace vg::graphics::driver::dx12
{
    class DepthStencilState : public base::DepthStencilState
    {
        using super = base::DepthStencilState;

    public:
        D3D12_DEPTH_STENCIL_DESC getd3d12DepthStencilState() const;
        
        static D3D12_COMPARISON_FUNC getd3d12ComparisonFunc(ComparisonFunc _cmpFunc);
        static D3D12_STENCIL_OP getd3d12StencilOp(StencilOp _stencilOp);

    private:
    };
}