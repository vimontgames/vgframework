#pragma once
#include "gfx/Device/vulkan/Extension/ExtensionList_vulkan.h"

namespace vg::gfx::vulkan
{
    struct KHR_RayQuery : public DeviceExtension
    {
        using super = DeviceExtension;
    public:
        bool init() override
        {
            return super::enable(VK_KHR_RAY_QUERY_EXTENSION_NAME);
        }

        bool create() override
        {
            if (isEnabled())
            {
                VG_VULKAN_DEVICE_PROC_ADRESS(CreateRayTracingPipelinesKHR);
                VG_VULKAN_DEVICE_PROC_ADRESS(GetAccelerationStructureDeviceAddressKHR);
                VG_VULKAN_DEVICE_PROC_ADRESS(GetRayTracingShaderGroupHandlesKHR);
                VG_VULKAN_DEVICE_PROC_ADRESS(CmdTraceRaysKHR);

                return true;
            }

            return false;
        }

        PFN_vkCreateRayTracingPipelinesKHR              m_pfnCreateRayTracingPipelinesKHR;
        PFN_vkGetAccelerationStructureDeviceAddressKHR  m_pfnGetAccelerationStructureDeviceAddressKHR;
        PFN_vkGetRayTracingShaderGroupHandlesKHR        m_pfnGetRayTracingShaderGroupHandlesKHR;
        PFN_vkCmdTraceRaysKHR                           m_pfnCmdTraceRaysKHR;

    };
}