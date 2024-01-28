#pragma once

#include "gfx/Shader/ShaderCompiler.h"

struct IDxcLibrary;
struct IDxcCompiler;
struct IDxcIncludeHandler;

namespace vg::gfx::dxc
{
    class ShaderCompiler : public base::ShaderCompiler
    {
        using super = base::ShaderCompiler;

    public:
        ShaderCompiler();
        ~ShaderCompiler();

    protected:
        core::string getDXCTargetProfile(ShaderStage _stage) const;
        gfx::Shader * compile(API _api, const core::string & _path, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, core::string & _warningAndErrors);

    private:
        IDxcLibrary *           m_d3d12dxcLibrary = nullptr;
        IDxcCompiler *          m_d3d12dxcCompiler = nullptr;
        IDxcIncludeHandler *    m_d3d12dxcIncludeHandler = nullptr;
    };
}