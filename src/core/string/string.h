#pragma once

#include "core/Types/Types.h"
#include "fmt/include/fmt/core.h"
#include "fmt/include/fmt/format.h" 
#include "fmt/include/fmt/printf.h" 

namespace vg::core
{
    core::string tolower(const core::string & _in);
    core::wstring wstring_convert(const core::string & _string);
    core::string string_convert(const core::wstring & _string);

    // Forward namespace ::fmt to core::fmt in case we change fmt implementation
    namespace fmt = ::fmt;
}



