#include "core/Precomp.h"
#include "file.h"
#include <iostream>
#include <fstream>

namespace vg::core::file
{
    //--------------------------------------------------------------------------------------
    bool read(const core::string & _path, core::string & _out)
    {
        using namespace std;

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

        VG_ASSERT(file.is_open(), "Could not read file \"%s\"", _path.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    string getFileDir(const core::string & _path)
    {
        size_t found = _path.find_last_of("/\\");
        return _path.substr(0, found);
    }
}