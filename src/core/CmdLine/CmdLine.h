#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
	class CmdLine
	{
	public:
		CmdLine(const string & _string);

		const string * find(const string & _key) const;

	private:
		map<string, string> m_args;
	};
}
