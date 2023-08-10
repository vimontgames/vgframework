#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
{
    class IShaderManager : core::Object
    {
    public:
        virtual core::uint GetShaderCompiledCount() const = 0;
        virtual core::uint GetShaderWarningCount() const = 0;
        virtual core::uint GetShaderErrorCount() const = 0;
    };
}