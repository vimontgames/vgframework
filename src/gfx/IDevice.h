#pragma once

#include "core/object/Object.h"
#include "gfx/device/Device_consts.h"

namespace vg::gfx
{
	class IDevice : public core::Object
	{
	public:
		virtual ~IDevice() {}
	};
}