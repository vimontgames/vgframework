#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    bool readFile(const core::string & _path, core::string & _out, bool _mustExist = true);
    string getFileDir(const core::string & _path);
}



