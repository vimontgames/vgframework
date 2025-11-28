#pragma once

#include "ImGuiAdapter.h"

namespace vg::renderer::vulkan
{
    class ImGuiAdapter : public vg::renderer::base::ImGuiAdapter
    {
        protected:
            void        init                    ();
            void        deinit                  ();

            void        beginFrame              ();
            void        render                  (gfx::CommandList * _cmdList);

            ImTextureID addTexture              (const gfx::Texture * _texture);
            void        removeTexture           (ImTextureID id);

            void        updateBackbufferFormat  ();

        private:
            void        createRenderPass        ();

        private:
            VkFormat            m_vkRenderTargetFormat;
            VkDescriptorPool    m_vkImguiDescriptorPool;
            VkRenderPass        m_vkImguiRenderPass;
            VkSampler           m_vkSampler;
    };
}