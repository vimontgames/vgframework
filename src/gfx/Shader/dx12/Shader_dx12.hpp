namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    Shader::Shader(const core::Blob & _bytecode) :
        super::Shader(_bytecode)
    {
        const uint size = _bytecode.size();
        m_d3d12bytecode.pShaderBytecode = malloc(size);
        memcpy((void*)m_d3d12bytecode.pShaderBytecode, _bytecode.data(), size);
        m_d3d12bytecode.BytecodeLength = size;
    }

    //--------------------------------------------------------------------------------------
    Shader::~Shader()
    {
        VG_SAFE_FREE((void*&)m_d3d12bytecode.pShaderBytecode);
        m_d3d12bytecode.BytecodeLength = 0;
    }
}