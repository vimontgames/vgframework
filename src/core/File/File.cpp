#include "core/Precomp.h"
#include "file.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool readFile(const core::string & _path, core::string & _out, bool _mustExist)
    {
        ifstream file(_path.c_str(), ios::in | ios::binary | ios::ate);

        if (file.is_open())
        {
            ifstream::pos_type fileSize = file.tellg();
            if (fileSize < 0)
            {
                file.close();
            }
            else
            {
                file.seekg(0, ios::beg);
                core::vector<char> bytes(fileSize);
                file.read(&bytes[0], fileSize);
                file.close();
                _out = string(&bytes[0], fileSize);
                return true;
            }
        }

        VG_ASSERT(!_mustExist || file.is_open(), "Could not read file \"%s\"", _path.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool writeFile(const core::string & _path, core::string & _in, bool _mustExist)
    {
        ofstream file(_path.c_str(), ios::out | ios::binary);

        if (file.is_open())
        {
            file.write(_in.c_str(), _in.size());
            file.close();
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    core::string getFileDir(const core::string & _path)
    {
        size_t found = _path.find_last_of("/\\");
        return _path.substr(0, found);
    }

    //--------------------------------------------------------------------------------------
    core::string findAndReplace(const core::string & _string, const core::string & _find, const std::string & _replace)
    {
        string res;

        const size_t length = _find.size();
        size_t pos;
        size_t from = 0;
        
        while (string::npos != (pos = _string.find(_find, from))) 
        {
            res.append(_string, from, pos - from);
            res.append(_replace);
            from = pos + length;
        }
        res.append(_string, from, string::npos);

        return res;
    }

    //--------------------------------------------------------------------------------------
    core::string cleanPath(const core::string & _path)
    {
        return findAndReplace(_path, "\\", "/");
    }

    //--------------------------------------------------------------------------------------
    core::string getCurrentWorkingDirectory()
    {
        const string syscwd = (std::filesystem::current_path()).string();
        const string path = cleanPath(syscwd);

        return path;
    }

    //--------------------------------------------------------------------------------------
    core::string getRelativePath(const core::string & _path)
    {
        auto path = cleanPath(_path);
        const auto cwd = core::getCurrentWorkingDirectory();

        const auto beginOffset = path.find(cwd);
        if (string::npos != beginOffset)
            path = path.substr(cwd.length()+1, path.length() - cwd.length()-1);

        return path;
    }
}