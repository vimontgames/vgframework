#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    core::string tolower(const core::string & _in);
    core::wstring wstring_convert(const core::string & _string);
    core::string string_convert(const core::wstring & _string);
}



