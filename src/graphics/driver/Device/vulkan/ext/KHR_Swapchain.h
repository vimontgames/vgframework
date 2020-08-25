#pragma once

#include "graphics/driver/Device/vulkan/Device_Extensions_vulkan.h"

namespace vg::graphics::driver::vulkan
{
	struct KHR_Swapchain : public DeviceExtension
	{
		using super = DeviceExtension;
	public:
		bool init() override
		{
			return super::enable(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

		bool create() override
		{
			if (isEnabled())
			{
				VG_VULKAN_DEVICE_PROC_ADRESS(GetSwapchainImagesKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(CreateSwapchainKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(DestroySwapchainKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(GetSwapchainImagesKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(AcquireNextImageKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(QueuePresentKHR);

				return true;
			}

			return false;
		}

		PFN_vkGetSwapchainImagesKHR	m_pfnGetSwapchainImagesKHR;
		PFN_vkCreateSwapchainKHR	m_pfnCreateSwapchainKHR;
		PFN_vkDestroySwapchainKHR	m_pfnDestroySwapchainKHR;
		PFN_vkAcquireNextImageKHR	m_pfnAcquireNextImageKHR;
		PFN_vkQueuePresentKHR		m_pfnQueuePresentKHR;
	};
}