#pragma once

#include "renderer/IImmediateGUI.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

typedef void * ImTextureID;

namespace vg
{
    namespace gfx
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
            ImguiAdapter(core::WinHandle _winHandle, gfx::Device & _device);
            ~ImguiAdapter();

            #ifdef VG_DX12
            void d3d12Init();
            #elif defined(VG_VULKAN)
            void vulkanInit();
            #endif

            bool IsKeyboardFocused() const override;
            bool IsMouseFocused() const override;

            void beginFrame();
            void render(gfx::CommandList * _cmdList);

            ImTextureID getImguiTextureID(gfx::Texture * _tex);
            void releaseImguiTextureID(ImTextureID _texID);

        private:
            gfx::BindlessTextureSrvHandle m_fontTexSRVHandle;

            #ifdef VG_VULKAN
            VkDescriptorPool                m_vkImguiDescriptorPool;
            VkRenderPass                    m_vkImguiRenderPass;
            VkSampler                       m_vkSampler;
            core::vector<VkDescriptorSet>   m_tempDescriptorSets;
            #endif
        };
    }
}