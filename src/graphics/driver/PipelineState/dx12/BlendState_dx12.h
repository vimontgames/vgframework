#pragma once

#include "graphics/driver/PipelineState/BlendState.h"

namespace vg::graphics::driver::dx12
{
    class BlendState : public base::BlendState
    {
        using super = base::BlendState;

    public:
        D3D12_BLEND_DESC getd3d12BlendState() const;
        
        static D3D12_BLEND getd3d12Blend(BlendFactor _blend);
        static D3D12_BLEND_OP getd3d12BlendOp(BlendOp _blendOp);

    private:
    };
}