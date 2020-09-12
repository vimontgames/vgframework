#include "graphics/driver/Precomp.h"
#include "shader.h"
#include "ShaderCompiler.h"
#include "ShaderManager.h"
#include "HLSLDesc.h"

#include "ShaderCompiler.hpp"
#include "ShaderManager.hpp"
#include "HLSLDesc.hpp"
#include "ShaderKey.hpp"

#include VG_GFXAPI_IMPL(Shader)

namespace vg::graphics::driver
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