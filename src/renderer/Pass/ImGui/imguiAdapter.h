#pragma once

#include "renderer/IImGuiAdapter.h"
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
        class ImGuiAdapter : public IImGuiAdapter
        {
        public:
                        ImGuiAdapter            (core::WinHandle _winHandle, gfx::Device & _device);
                        ~ImGuiAdapter           ();

            #ifdef VG_DX12
            void        d3d12Init               ();
            #elif defined(VG_VULKAN)
            void        vulkanInit              ();
            #endif

            bool        IsKeyboardFocused       () const final override;
            bool        IsMouseFocused          () const final override;

            ImTextureID GetTextureID            (const gfx::ITexture * _texture) const final override;
            void        ReleaseTextureID        (ImTextureID _texID) final override;

            void        beginFrame              ();
            void        render                  (gfx::CommandList * _cmdList);

        protected:
            ImTextureID getImguiTextureID       (gfx::Texture * _tex) const;
            void        releaseImguiTextureID   (ImTextureID _texID);

        private:
            gfx::BindlessTextureSrvHandle       m_fontTexSRVHandle;

            #ifdef VG_VULKAN
            VkDescriptorPool                    m_vkImguiDescriptorPool;
            VkRenderPass                        m_vkImguiRenderPass;
            VkSampler                           m_vkSampler;
            core::vector<VkDescriptorSet>       m_tempDescriptorSets;
            #endif
        };
    }
}