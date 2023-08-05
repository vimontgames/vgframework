#pragma once

#include "gfx/PipelineState/SamplerState.h"

namespace vg::gfx::dx12
{
    class SamplerState : public base::SamplerState
    {
        using super = base::SamplerState;

    public:
        static D3D12_STATIC_SAMPLER_DESC getd3d12SamplerState(Sampler _sampler);

        static D3D12_FILTER                 getd3d12Filter(Filter _filter);
        static D3D12_TEXTURE_ADDRESS_MODE   getd3d12Adress(Address _address);

    private:
    };
}