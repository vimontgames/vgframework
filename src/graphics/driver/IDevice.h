#pragma once

#include "core/object/Object.h"

namespace vg::graphics::driver
{
	enum class API
	{
		DirectX12,
		Vulkan,
		Metal
	};

	struct DeviceParams
	{
		core::WinHandle			window			= core::invalidWindowHandle;
		core::InstanceHandle	instance		= core::invalidInstanceHandle;
		API						api				= API::DirectX12;
		core::uint2				resolution		= { 1280,720 };
		bool					debugDevice		= false;
		bool					breakOnErrors	= false;
		bool					breakOnWarnings	= false;
	};

	class IDevice : public core::Object
	{
	public:
		virtual ~IDevice() {}
	};
}