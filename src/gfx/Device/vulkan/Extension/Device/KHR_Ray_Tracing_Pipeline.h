#pragma once

#include "gfx/Device/vulkan/Extension/ExtensionList_vulkan.h"

namespace vg::gfx::vulkan
{
    struct KHR_RayTracingPipeline : public DeviceExtension
    {
        using super = DeviceExtension;
    public:
        bool init() override
        {
            return super::enable(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
        }

        bool create() override
        {
            if (isEnabled())
            {


                return true;
            }

            return false;
        }

    };
}