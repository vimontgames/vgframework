#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
	class CmdLine
	{
	public:
		CmdLine(const string & _string);

		bool getString(const string & _key, string & _value) const;
		bool getBool(const string & _key, bool & _value) const;

	private:
		map<string, string> m_args;
	};
}
