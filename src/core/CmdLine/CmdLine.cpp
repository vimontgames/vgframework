#include "core/Precomp.h"
#include "CmdLine.h"
#include "core/Types/Types.h"
#include "core/Math/Math.h"
#include "core/Kernel.h"
#include "core/string/string.h"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	CmdLine::CmdLine(const string & _string)
	{
		if (!_string.empty())
		{
			size_t offset = 0;

			while (string::npos != offset)
			{
				auto keyOffset = _string.find_first_not_of(" ", offset);
				
				if (string::npos != keyOffset)
				{
					auto keyEndOffset = _string.find("=", offset);
					string key = _string.substr(keyOffset, keyEndOffset - keyOffset);

					// string?
					size_t beginOffset = _string.find("=\"", keyOffset);
					size_t endOffset;
					if (string::npos != beginOffset)
					{
						endOffset = _string.find("\"", beginOffset + 2);

						if (string::npos != beginOffset && string::npos != endOffset)
						{
							string value = _string.substr(beginOffset + 2, endOffset - (beginOffset + 2));
							m_args.insert({ key, value });
						}
					}
					else
					{
						endOffset = _string.find(' ', offset);
						endOffset = core::min(endOffset, _string.find('\n', keyOffset));
						endOffset = core::min(endOffset, _string.length());

						if (string::npos != endOffset)
						{
							string value = _string.substr(keyEndOffset + 1, endOffset - keyEndOffset - 1);
							m_args.insert({ key, value });
						}
					}

					offset = endOffset + 1;
				}
                else
                    break;
			}
		}
	}

	//--------------------------------------------------------------------------------------
	bool CmdLine::getString(const string & _key, string & _value) const
	{
		auto it = m_args.find(_key);
		if (m_args.end() != it)
		{
			_value = it->second;
			return true;
		}
		else
		{
			VG_INFO("[CmdLine] Parameter \"%s\" not found in command-line", _key.c_str());
			return false;
		}
	}

    //--------------------------------------------------------------------------------------
	bool CmdLine::getBool(const string & _key, bool & _value) const
	{
		string s;
		if (getString(_key, s))
		{
			if (core::tolower(s) == "true" || s == "1")
				_value = true;

			if (core::tolower(s) == "false" || s == "0")
				_value = false;

			return true;
		}

		return false;
	}

    //--------------------------------------------------------------------------------------
    bool CmdLine::getInt(const string & _key, int & _value) const
	{
		string s;
		if (getString(_key, s))
		{
            errno = 0; 
			const char * begin = s.c_str();
			const char * end = s.c_str() + s.length();
			_value = std::strtol(begin, (char**)&end, 10);

			if (errno != ERANGE)
				return true;			
		}

		return false;
	}
}