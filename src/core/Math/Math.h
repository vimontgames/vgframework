#pragma once

#include "core/Types/Types.h"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	template <typename T> T min(const T & _left, const T & _right)
	{
		return _left < _right ? _left : _right;
	}

	//--------------------------------------------------------------------------------------
	template <typename T> T max(const T & _left, const T & _right)
	{
		return _left > _right ? _left : _right;
	}
}