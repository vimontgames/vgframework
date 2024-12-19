#if VG_ENABLE_SHADER_COMPILATION

#include "ShaderCompiler.h"
#include "core/Container/Blob.h"
#include "core/File/File.h"

using namespace vg::core;

#include "dxc/ShaderCompiler_dxc.hpp"

namespace vg::gfx
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
    core::Blob ShaderCompiler::compile(API _api, const string & _path, const string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, string & _warningAndErrors)
    {
        return super::compile(_api, _path, _entryPoint, _stage, _macros, _warningAndErrors);
    }
}

#endif // VG_ENABLE_SHADER_COMPILATION