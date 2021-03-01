#pragma once

#include "graphics/driver/PipelineState/DepthStencilState.h"

namespace vg::graphics::driver::vulkan
{
    class DepthStencilState : public base::DepthStencilState
    {
        using super = base::DepthStencilState;

    public:
        //D3D12_BLEND_DESC getd3d12BlendState() const;
        //
        //static D3D12_BLEND getd3d12Blend(BlendFactor _blend);
        //static D3D12_BLEND_OP getd3d12BlendOp(BlendOp _blendOp);

    private:
    };
}