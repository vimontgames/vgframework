#pragma once

#include "graphics/driver/Device/vulkan/Device_Extensions_vulkan.h"

namespace vg::graphics::driver::vulkan
{
	struct EXT_DebugUtils : public InstanceExtension
	{
		using super = InstanceExtension;
	public:
		bool init() override
		{
			return super::enable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		bool create() override
		{
			if (isEnabled())
			{
				// Setup VK_EXT_debug_utils function pointers always (we use them for debug labels and names).
				VG_VULKAN_INSTANCE_PROC_ADRESS(CreateDebugUtilsMessengerEXT);
				VG_VULKAN_INSTANCE_PROC_ADRESS(DestroyDebugUtilsMessengerEXT);
				VG_VULKAN_INSTANCE_PROC_ADRESS(SubmitDebugUtilsMessageEXT);
				VG_VULKAN_INSTANCE_PROC_ADRESS(CmdBeginDebugUtilsLabelEXT);
				VG_VULKAN_INSTANCE_PROC_ADRESS(CmdEndDebugUtilsLabelEXT);
				VG_VULKAN_INSTANCE_PROC_ADRESS(CmdInsertDebugUtilsLabelEXT);
				VG_VULKAN_INSTANCE_PROC_ADRESS(SetDebugUtilsObjectNameEXT);

				return true;
			}

			return false;
		}

		PFN_vkCreateDebugUtilsMessengerEXT	m_pfnCreateDebugUtilsMessengerEXT;
		PFN_vkDestroyDebugUtilsMessengerEXT m_pfnDestroyDebugUtilsMessengerEXT;
		PFN_vkSubmitDebugUtilsMessageEXT	m_pfnSubmitDebugUtilsMessageEXT;
		PFN_vkCmdBeginDebugUtilsLabelEXT	m_pfnCmdBeginDebugUtilsLabelEXT;
		PFN_vkCmdEndDebugUtilsLabelEXT		m_pfnCmdEndDebugUtilsLabelEXT;
		PFN_vkCmdInsertDebugUtilsLabelEXT	m_pfnCmdInsertDebugUtilsLabelEXT;
		PFN_vkSetDebugUtilsObjectNameEXT	m_pfnSetDebugUtilsObjectNameEXT;
	};
}