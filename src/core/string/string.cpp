#include "core/Precomp.h"
#include "string.h"
#include <locale>

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // convert string to lowercase
    //--------------------------------------------------------------------------------------
    string tolower(const string & _in)
    {
        std::locale loc;

        string out;
        for (uint i = 0; i < _in.length(); ++i)
            out += std::tolower(_in[i], loc);

        return out;
    }

    //--------------------------------------------------------------------------------------
    // convert string to wstring
    //--------------------------------------------------------------------------------------
    wstring wstring_convert(const string & _string)
    {
        int len;
        int slength = (int)_string.length() + 1;
        len = MultiByteToWideChar(CP_ACP, 0, _string.c_str(), slength, 0, 0);
        std::wstring r(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, _string.c_str(), slength, &r[0], len);
        return r;
    }

    //--------------------------------------------------------------------------------------
    // convert string to wstring
    //--------------------------------------------------------------------------------------
    string string_convert(const wstring & _string)
    {
        int len;
        int slength = (int)_string.length() + 1;
        len = WideCharToMultiByte(CP_ACP, 0, _string.c_str(), slength, 0, 0, 0, 0);
        std::string r(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, _string.c_str(), slength, &r[0], len, 0, 0);
        return r;
    }
}