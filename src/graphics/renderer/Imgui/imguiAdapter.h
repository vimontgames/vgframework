#pragma once

#include "graphics/renderer/IImmediateGUI.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"

typedef void* ImTextureID;

namespace vg::graphics
{
    namespace driver
    {
        class Device;
        class Texture;
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

            bool IsKeyboardFocused() const override;
            bool IsMouseFocused() const override;

            void beginFrame();
            void render(driver::CommandList * _cmdList);

            ImTextureID getImguiTextureID(driver::Texture * _tex);
            void releaseImguiTextureID(ImTextureID _texID);

        private:
            driver::BindlessTextureSrvHandle m_fontTexSRVHandle;

            #ifdef VG_VULKAN
            VkDescriptorPool                m_vkImguiDescriptorPool;
            VkRenderPass                    m_vkImguiRenderPass;
            VkSampler                       m_vkSampler;
            core::vector<VkDescriptorSet>   m_tempDescriptorSets;
            #endif
        };
    }
}