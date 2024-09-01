#pragma once

namespace vg::gfx::vulkan
{
    struct EXT_Swapchain_Colorspace : public InstanceExtension
    {
        using super = InstanceExtension;

    public:
        bool init() override
        {
            return super::enable(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
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