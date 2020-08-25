#include "core/Precomp.h"
#include "CmdLine.h"
#include "core/Types/Types.h"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	CmdLine::CmdLine(const string & _string)
	{
		if (!_string.empty())
		{
			size_t offset = 0;

			while (true)
			{
				auto keyOffset = _string.find_first_not_of(" ", offset);
				
				if (string::npos != keyOffset)
				{
					auto keyEndOffset = _string.find("=", offset);
					string key = _string.substr(keyOffset, keyEndOffset - keyOffset);

					auto beginOffset = _string.find("=\"", keyOffset);
					auto endOffset = _string.find("\"", beginOffset + 2);

					if (string::npos != beginOffset && string::npos != endOffset)
					{
						string value = _string.substr(beginOffset + 2, endOffset - (beginOffset + 2));
						m_args.insert({ key, value });

						offset = endOffset + 1;
					}
				}
				else
					break;
			}
		}
	}

	//--------------------------------------------------------------------------------------
	const string * CmdLine::find(const string & _key) const
	{
		auto it = m_args.find("SolutionPlatform");
		if (m_args.end() != it)
			return &it->second;
		else
			return nullptr;
	}
}