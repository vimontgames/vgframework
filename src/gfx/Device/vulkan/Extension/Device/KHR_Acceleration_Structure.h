#pragma once

namespace vg::gfx::vulkan
{
	struct KHR_Acceleration_Structure : public DeviceExtension
	{
		using super = DeviceExtension;

	public:
		bool init() override
		{
			return super::enable(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
		}

		bool create() override
		{
			if (isEnabled())
			{
				VG_VULKAN_DEVICE_PROC_ADRESS(GetAccelerationStructureBuildSizesKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(CreateAccelerationStructureKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(DestroyAccelerationStructureKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(GetAccelerationStructureDeviceAddressKHR);
				VG_VULKAN_DEVICE_PROC_ADRESS(CmdBuildAccelerationStructuresKHR);

				return true;
			}

			return false;
		}

        PFN_vkGetAccelerationStructureBuildSizesKHR		m_pfnGetAccelerationStructureBuildSizesKHR;
		PFN_vkCreateAccelerationStructureKHR			m_pfnCreateAccelerationStructureKHR;
		PFN_vkDestroyAccelerationStructureKHR			m_pfnDestroyAccelerationStructureKHR;
		PFN_vkGetAccelerationStructureDeviceAddressKHR	m_pfnGetAccelerationStructureDeviceAddressKHR;
		PFN_vkCmdBuildAccelerationStructuresKHR			m_pfnCmdBuildAccelerationStructuresKHR;

	};
}