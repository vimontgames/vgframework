#pragma once

namespace vg::gfx::vulkan
{
    struct EXT_HDR_Metadata : public DeviceExtension
    {
        using super = DeviceExtension;

    public:
        bool init() override
        {
            return super::enable(VK_EXT_HDR_METADATA_EXTENSION_NAME);
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