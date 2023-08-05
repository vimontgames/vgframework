#pragma once

#include "gfx/Device/vulkan/Device_Extensions_vulkan.h"

namespace vg::gfx::vulkan
{
	struct KHR_Surface : public InstanceExtension
	{
		using super = InstanceExtension;
	public:
		bool init() override
		{
			return super::enable(VK_KHR_SURFACE_EXTENSION_NAME);
		}

		bool create() override
		{
			if (isEnabled())
			{
				VG_VULKAN_INSTANCE_PROC_ADRESS(GetPhysicalDeviceSurfaceSupportKHR);
				VG_VULKAN_INSTANCE_PROC_ADRESS(GetPhysicalDeviceSurfaceCapabilitiesKHR);
				VG_VULKAN_INSTANCE_PROC_ADRESS(GetPhysicalDeviceSurfaceFormatsKHR);
				VG_VULKAN_INSTANCE_PROC_ADRESS(GetPhysicalDeviceSurfacePresentModesKHR);

				return true;
			}

			return false;
		}

		PFN_vkGetPhysicalDeviceSurfaceSupportKHR		m_pfnGetPhysicalDeviceSurfaceSupportKHR;
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	m_pfnGetPhysicalDeviceSurfaceCapabilitiesKHR;
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		m_pfnGetPhysicalDeviceSurfaceFormatsKHR;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	m_pfnGetPhysicalDeviceSurfacePresentModesKHR;
	};
}