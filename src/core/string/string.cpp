#include "core/Precomp.h"
#include "string.h"
#include <locale>
#include "fmt/src/format.cc"

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
        int slength = (int)_string.length();
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
        int slength = (int)_string.length();
        len = WideCharToMultiByte(CP_ACP, 0, _string.c_str(), slength, 0, 0, 0, 0);
        std::string r(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, _string.c_str(), slength, &r[0], len, 0, 0);
        return r;
    }

    //--------------------------------------------------------------------------------------
    // get lines from string
    //--------------------------------------------------------------------------------------
    vector<string> getLines(const string & _string)
    {
        vector<string> lines;
        size_t startPos = 0;
        size_t endPos;

        while ((endPos = _string.find('\n', startPos)) != std::string::npos)
        {
            std::string line = _string.substr(startPos, endPos - startPos);
            lines.push_back(line);

            startPos = endPos + 1;
        }

        // last line
        if (startPos < _string.length())
        {
            std::string lastLine = _string.substr(startPos);
            lines.push_back(lastLine);
        }

        return lines;
    }

    //--------------------------------------------------------------------------------------
    bool endsWith(const string & _str, const string & _suffix)
    {
        return _str.size() >= _suffix.size() && _str.compare(_str.size() - _suffix.size(), _suffix.size(), _suffix) == 0;
    }

    //--------------------------------------------------------------------------------------
    string removeTrailingChar(const string & _str, const char _char)
    {
        string str = _str;
        if (!str.empty() && str.back() == _char)
            str.pop_back();
        return str;
    }

    //--------------------------------------------------------------------------------------
    string removeTrailingString(const core::string & _str, const core::string _suffix)
    {
        if (endsWith(_str, _suffix)) 
        {
            string str = _str;
            str.erase(str.size() - _suffix.size());
            return str;
        }
        return _str;
    }
}