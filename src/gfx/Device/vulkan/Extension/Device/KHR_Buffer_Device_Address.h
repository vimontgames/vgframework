#pragma once
#include "gfx/Device/vulkan/Extension/ExtensionList_vulkan.h"

namespace vg::gfx::vulkan
{
    struct KHR_BufferDeviceAddress : public DeviceExtension
    {
        using super = DeviceExtension;
    public:
        bool init() override
        {
            return super::enable(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        }

        bool create() override
        {
            if (isEnabled())
            {
                VG_VULKAN_DEVICE_PROC_ADRESS(GetBufferDeviceAddressKHR);

                return true;
            }

            return false;
        }

        PFN_vkGetBufferDeviceAddressKHR     m_pfnGetBufferDeviceAddressKHR;
    };
}