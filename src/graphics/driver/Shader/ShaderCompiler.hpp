#if VG_ENABLE_SHADER_COMPILATION

#include "ShaderCompiler.h"
#include "core/Container/Blob.h"
#include "core/File/File.h"

using namespace vg::core;

#include "dxc/ShaderCompiler_dxc.hpp"

namespace vg::graphics::driver
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        ShaderCompiler::ShaderCompiler()
        {

        }

        //--------------------------------------------------------------------------------------
        ShaderCompiler::~ShaderCompiler()
        {

        }
    }

    //--------------------------------------------------------------------------------------
    ShaderCompiler::ShaderCompiler()
    {

    }

    //--------------------------------------------------------------------------------------
    ShaderCompiler::~ShaderCompiler()
    {

    }

    //--------------------------------------------------------------------------------------
    Shader * ShaderCompiler::compile(API _api, const string & _path, const string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, string & _warningAndErrors)
    {
        auto * shader = super::compile(_api, _path, _entryPoint, _stage, _macros,_warningAndErrors);

        if (shader)
            shader->setName(_entryPoint);

        return shader;
    }
}

#endif // VG_ENABLE_SHADER_COMPILATION