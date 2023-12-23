#pragma once

#include "core/Types/Types.h"
#include "core/string/string.h"

namespace vg::core
{
	class CmdLine
	{
	public:
		CmdLine(const string & _string);

		bool getString(const string & _key, string & _value) const;
		bool getBool(const string & _key, bool & _value) const;

	private:
        struct CaseInsensitive
        {
            bool operator()(const std::string & a, const std::string & b) const noexcept
            {
				return core::tolower(a).compare(core::tolower(b)) < 0;
            }
        };

		std::map<string, string, CaseInsensitive> m_args;
	};
}
