#include "gfx/Precomp.h"
#include "core/string/string.h"
#include "gfx/device/Device.h"
#include "shader.h"
#include "ShaderCompiler.h"
#include "ShaderManager.h"
#include "HLSLDesc.h"

using namespace vg::core;

#include "ShaderCompiler.hpp"
#include "ShaderManager.hpp"
#include "HLSLDesc.hpp"
#include "ShaderKey.hpp"
#include "ComputeShaderKey.hpp"
#include "RayTracingShaderKey.hpp"

#include VG_GFXAPI_IMPL(Shader)

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