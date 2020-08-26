#pragma once

namespace vg::core
{
	void debugPrint(const char * _format, ...);

	bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip, const char * _format, ...);
	bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip);
}