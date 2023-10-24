#pragma once

#include "gfx/Device/vulkan/Extension/ExtensionList_vulkan.h"

namespace vg::gfx::vulkan
{
	struct KHR_Win32_Surface : public InstanceExtension
	{
		using super = InstanceExtension;
	public:
		bool init() override
		{
			return super::enable(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		}

		bool create() override
		{
			return isEnabled();
		}
	};
}