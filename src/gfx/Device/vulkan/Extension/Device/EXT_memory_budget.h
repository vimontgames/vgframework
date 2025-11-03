#pragma once

namespace vg::gfx::vulkan
{
    struct EXT_MemoryBudget : public DeviceExtension
    {
        using super = DeviceExtension;

    public:
        bool init() override
        {
            return super::enable(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
        }

        bool create() override
        {
            if (isEnabled())
            {
                VG_VULKAN_DEVICE_PROC_ADRESS(GetAccelerationStructureBuildSizesKHR);

                return true;
            }

            return false;
        }

        PFN_vkGetAccelerationStructureBuildSizesKHR		m_pfnGetAccelerationStructureBuildSizesKHR;
    };
}