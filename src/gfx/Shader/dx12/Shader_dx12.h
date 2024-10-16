#pragma once

#include "gfx/Shader/Shader.h"

namespace vg::gfx::dx12
{
    class Shader : public base::Shader
    {
        using super = base::Shader;

    public:
        Shader(const core::Blob & _bytecode);
        ~Shader();

        const D3D12_SHADER_BYTECODE & getd3d12Bytecode() const { return m_d3d12bytecode; }

    private:
        D3D12_SHADER_BYTECODE   m_d3d12bytecode = {};
    };
}