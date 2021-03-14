#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
    bool readFile(const core::string & _path, core::string & _out, bool _mustExist = true);
    bool writeFile(const core::string & _path, core::string & _in, bool _mustExist = false);

    core::string getFileDir(const core::string & _path);
    core::string getCurrentWorkingDirectory();
    core::string getRelativePath(const core::string & _path);
    core::string cleanPath(const core::string & _path);
    core::string findAndReplace(const core::string & _string, const core::string & _find, const std::string & _replace);
}



