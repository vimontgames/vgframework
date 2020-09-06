#pragma once

#include "core/Types/Types.h"

namespace vg::core::file
{
    bool read(const core::string & _path, core::string & _out);
    string getFileDir(const core::string & _path);
}



