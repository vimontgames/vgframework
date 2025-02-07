#include "gfx/Precomp.h"
#include "core/string/string.h"
#include "gfx/Device/Device.h"
#include "shader.h"
#include "ShaderCompiler.h"
#include "ShaderManager.h"
#include "HLSLDesc.h"

#include "ShaderCompiler.hpp"
#include "ShaderManager.hpp"
#include "HLSLDesc.hpp"
#include "ShaderKey.hpp"
#include "ComputeShaderKey.hpp"

#ifdef VG_DX12
#include "dx12/Shader_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/Shader_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        Shader::Shader(const core::Blob & _bytecode)
        {

        }

        //--------------------------------------------------------------------------------------
        Shader::~Shader()
        {

        }
    }

    //--------------------------------------------------------------------------------------
    Shader::Shader(const core::Blob & _bytecode) :
        super::Shader(_bytecode)
    {

    }

    //--------------------------------------------------------------------------------------
    Shader::~Shader()
    {

    }
}