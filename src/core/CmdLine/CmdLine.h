#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
	class CmdLine
	{
	public:
		CmdLine(const string & _string);

		string getString(const string & _key) const;
		bool getBool(const string & _key) const;

	private:
		map<string, string> m_args;
	};
}
