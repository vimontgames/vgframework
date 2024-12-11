#pragma once

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
                return true;
            }

            return false;
        }
    };
}