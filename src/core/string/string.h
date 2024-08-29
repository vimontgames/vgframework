#pragma once

#include "core/Types/Types.h"
#include "fmt/include/fmt/core.h"
#include "fmt/include/fmt/format.h" 
#include "fmt/include/fmt/printf.h" 

namespace vg::core
{
    // Convert string to lowercase
    core::string tolower(const core::string & _in);

    // Convert between string and wide string
    core::wstring wstring_convert(const core::string & _string);
    core::string string_convert(const core::wstring & _string);

    // Get an array of lines from a single string
    core::vector<core::string> getLines(const core::string & _string);

    // Check file extension
    bool endsWith(const core::string & str, const core::string & suffix);

    // Forward namespace ::fmt to core::fmt in case we change fmt implementation
    namespace fmt = ::fmt;
}



