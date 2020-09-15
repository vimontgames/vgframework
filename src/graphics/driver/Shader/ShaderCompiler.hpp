#if VG_ENABLE_SHADER_COMPILATION

#include "ShaderCompiler.h"
#include "core/Container/Blob.h"
#include "core/File/File.h"

using namespace vg::core;

#include VG_GFXAPI_IMPL(ShaderCompiler)

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
    Shader * ShaderCompiler::compile(const core::string & _path, const core::string & _entryPoint, ShaderStage _stage)
    {
        auto * shader = super::compile(_path, _entryPoint, _stage);

        if (shader)
            shader->setName(_entryPoint);

        return shader;
    }
}

#endif // VG_ENABLE_SHADER_COMPILATION