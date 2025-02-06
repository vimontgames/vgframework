#pragma once

#include "core/Object/Object.h"
#include "gfx/Device/Device_consts.h"

namespace vg::gfx
{
	class IDevice : public core::Object
	{
	public:
		virtual ~IDevice() {}
	};
}