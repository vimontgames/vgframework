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
        wchar_t result[1024];
        VG_ASSERT(_string.length() < countof(result));
        size_t s;
        mbstowcs_s(&s, result, countof(result), _string.c_str(), _string.length());
        return wstring(result);
    }
}