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
    ShaderCompiler::ShaderCompiler(const core::string & _shaderRootPath) :
        m_shaderRootPath(_shaderRootPath)
    {
        // Path must end with '\' or '/'
        if (m_shaderRootPath.back() != '/' && m_shaderRootPath.back() != '\\')
            m_shaderRootPath += "/";
    }

    //--------------------------------------------------------------------------------------
    ShaderCompiler::~ShaderCompiler()
    {

    }

    //--------------------------------------------------------------------------------------
    Shader * ShaderCompiler::compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage)
    {
        const string path = m_shaderRootPath + _file;
        return super::compile(path, _entryPoint, _stage);
    }
}

#endif // VG_ENABLE_SHADER_COMPILATION