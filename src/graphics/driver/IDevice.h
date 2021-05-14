#pragma once

#include "core/object/Object.h"
#include "graphics/driver/device/Device_consts.h"

namespace vg::graphics::driver
{
	class IDevice : public core::ObjectPointer
	{
	public:
		virtual ~IDevice() {}
	};
}