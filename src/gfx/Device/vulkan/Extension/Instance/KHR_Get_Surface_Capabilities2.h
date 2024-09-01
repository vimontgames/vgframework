#pragma once

namespace vg::gfx::vulkan
{
    struct KHR_Get_Surface_Capabilities2 : public InstanceExtension
    {
        using super = InstanceExtension;

    public:
        bool init() override
        {
            return super::enable(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
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