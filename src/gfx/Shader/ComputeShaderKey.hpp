#include "ComputeShaderKey.h"
#include "gfx/Shader/ShaderManager.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    ComputeShaderKey::ComputeShaderKey()
    {
        clear();
    }

    //--------------------------------------------------------------------------------------
    void ComputeShaderKey::clear()
    {
        cs = ComputeShaderKey::CS(-1);
        flags = ComputeShaderKey::Flags(0x0000);
    }

    //--------------------------------------------------------------------------------------
    ComputeShaderKey::ComputeShaderKey(const core::string & _file, const core::string & _technique)
    {
        clear();
        init(_file, _technique);
    }

    //--------------------------------------------------------------------------------------
    ComputeShaderKey::~ComputeShaderKey()
    {

    }

    //--------------------------------------------------------------------------------------
    void ComputeShaderKey::init(const core::string & _file, const core::string & _technique)
    {
        ShaderManager::get()->initComputeShaderKey(*this, _file, _technique);
    }

}