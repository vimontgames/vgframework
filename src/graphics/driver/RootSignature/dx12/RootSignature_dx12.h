#pragma once

#include "graphics/driver/RootSignature/RootSignature.h"

namespace vg::graphics::driver::dx12
{
    class RootSignature : public base::RootSignature
    {
        using super = base::RootSignature;

    public:
        RootSignature(const RootSignatureDesc & _desc);
        ~RootSignature();

        ID3D12RootSignature * getd3d12RootSignature() const { return m_d3d12rootSignature; }

        static D3D12_SHADER_VISIBILITY getd3d12ShaderStageFlags(ShaderStageFlags _shaderStageFlags);

    private:
        ID3D12RootSignature *   m_d3d12rootSignature;
    };
}