#pragma once

#include "imgui/imgui.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"

namespace vg::graphics
{
    namespace driver
    {
        class Device;
        class CommandList;
    }

    namespace renderer
    {
        class ImguiAdapter
        {
        public:
            ImguiAdapter(core::WinHandle _winHandle, driver::Device & _device);
            ~ImguiAdapter();

            #ifdef VG_DX12
            void d3d12Init();
            #elif defined(VG_VULKAN)
            void vulkanInit();
            #endif

            void beginFrame();
            void render(driver::CommandList * _cmdList);

        private:
            driver::BindlessTextureSrvHandle m_fontTexSRVHandle;

            #ifdef VG_DX12
            #elif defined(VG_VULKAN)
            VkDescriptorPool m_vkImguiDescriptorPool;
            VkRenderPass m_vkImguiRenderPass;
            #endif
        };
    }
}