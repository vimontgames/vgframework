#pragma once

#include "gfx/PipelineState/Graphic/RasterizerState.h"

namespace vg::gfx::dx12
{
    class RasterizerState : public base::RasterizerState
    {
        using super = base::RasterizerState;

    public:
        D3D12_RASTERIZER_DESC getd3d12RasterizerState() const;

        static D3D12_FILL_MODE getd3d12FillMode(FillMode _fillMode);
        static D3D12_CULL_MODE getd3d12CullMode(CullMode _cullMode);

    private:
    };
}