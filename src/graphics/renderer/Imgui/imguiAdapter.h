#pragma once

#include "graphics/renderer/IImmediateGUI.h"
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
        class ImguiAdapter : public IImmediateGUI
        {
        public:
            ImguiAdapter(core::WinHandle _winHandle, driver::Device & _device);
            ~ImguiAdapter();

            #ifdef VG_DX12
            void d3d12Init();
            #elif defined(VG_VULKAN)
            void vulkanInit();
            #endif

            bool isFocused() const override;

            void beginFrame();
            void render(driver::CommandList * _cmdList);

        private:
            driver::BindlessTextureSrvHandle m_fontTexSRVHandle;

            #ifdef VG_VULKAN
            VkDescriptorPool m_vkImguiDescriptorPool;
            VkRenderPass m_vkImguiRenderPass;
            #endif
        };
    }
}