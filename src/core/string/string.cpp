#include "core/Precomp.h"
#include "string.h"
#include <locale>

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    string tolower(const string & _in)
    {
        std::locale loc;

        string out;
        for (uint i = 0; i < _in.length(); ++i)
            out += std::tolower(_in[i], loc);

        return out;
    }
}