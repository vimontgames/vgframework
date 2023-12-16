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
	string CmdLine::getString(const string & _key) const
	{
		auto it = m_args.find(_key);
		if (m_args.end() != it)
			return it->second;

		VG_WARNING("[CmdLine] Could not find key \"%s\" is command-line", _key);
		return "";
	}

    //--------------------------------------------------------------------------------------
	bool CmdLine::getBool(const string & _key) const
	{
		string value = getString(_key);
		if (core::tolower(value) == "true" || value == "1")
			return true;

		if (core::tolower(value) == "false" || value == "0")
			return false;

		return false;
	}
}