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
        class ImGuiAdapter final : public IImGuiAdapter
        {
        public:
                        ImGuiAdapter            (core::WinHandle _winHandle, gfx::Device & _device);
                        ~ImGuiAdapter           ();

            #ifdef VG_DX12
            void        d3d12Init               ();
            #elif defined(VG_VULKAN)
            void        vulkanInit              ();
            #endif

            void        SetGUITheme             (GUITheme _theme) final override;

            bool        IsKeyboardFocused       () const final override;
            bool        IsMouseFocused          () const final override;

            ImTextureID GetTextureID            (const gfx::ITexture * _texture) const final override;
            void        ReleaseTextureID        (ImTextureID _texID) final override;

            void        beginFrame              ();
            void        render                  (gfx::CommandList * _cmdList);

            void        AddBeginFrameCallback   (BeginFrameCallback _func);

        protected:
            ImTextureID getTextureID            (gfx::Texture * _tex) const;
            void        releaseTextureID        (ImTextureID _texID);

            void        resetGUITheme           ();
            void        setGUIThemeClassic      ();
            void        setGUIThemeDark         ();
            void        setGUIThemeLight        ();
            void        setGUIThemeVGDark       ();

        private:
            gfx::BindlessTextureHandle          m_fontTexHandle;

            core::vector<BeginFrameCallback>    m_beginFrameCallbacks;

            #ifdef VG_VULKAN
            VkDescriptorPool                    m_vkImguiDescriptorPool;
            VkRenderPass                        m_vkImguiRenderPass;
            VkSampler                           m_vkSampler;
            core::vector<VkDescriptorSet>       m_tempDescriptorSets[2];
            #endif
        };
    }
}