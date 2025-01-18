#pragma once

#include "renderer/IImGuiAdapter.h"
#include "renderer/RenderPass/Render2D/Preview/Texture/TexturePreview.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"
#include "ImGuiSettings.h"

typedef vg::core::u64 ImTextureID;

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
            ImGuiAdapter(core::WinHandle _winHandle, gfx::Device & _device);
            ~ImGuiAdapter();

            #ifdef VG_DX12
            void                    d3d12Init                       ();
            #elif defined(VG_VULKAN)
            void                    vulkanInit                      ();
            #endif

            void                    SetGUITheme                     (Theme _theme) final override;
            bool                    IsCurrentThemeDark              () const final override;

            const ImVec4 &          GetTextColor                    () const final override { return m_textColor; }
            const ImVec4 &          GetTransientPropertyColor       () const final override { return m_unsavedPropertyColor; }
            const ImVec4 &          GetPrefabOverridePropertyColor  () const final override { return m_prefabOverridePropertyColor; }

            const ImVec4 &          GetRowColorEven                 () const final override { return m_rowColorEven;}
            const ImVec4 &          GetRowColorOdd                  () const final override { return m_rowColorOdd; }

            const ImVec4 &          GetWarningColor                 () const final override { return m_warningColor; }
            const ImVec4 &          GetErrorColor                   () const final override { return m_errorColor; }

            bool                    IsKeyboardFocused               () const final override;
            bool                    IsMouseFocused                  () const final override;

            ImTextureID             GetTextureID                    (const gfx::ITexture * _texture) final override;
            void                    ReleaseTextureID                (const gfx::ITexture * _texture) final override;

            void                    beginFrame                      (gfx::CommandList * _cmdList);
            void                    render                          (gfx::CommandList * _cmdList);

            void                    AddBeginFrameCallback           (BeginFrameCallback _func);
            ImFont *                GetFont                         (Font _font, FontStyle _style = FontStyle::Regular) final override;
            const char *            GetFontPath                     (Font _font, FontStyle _style) const final override;

            void                    PushDefaultFont                 () final override;
            void                    PushFont                        (vg::renderer::Font _font) final override;
            void                    PushFont                        (vg::renderer::Font _font, vg::renderer::FontStyle _style) final override;
            void                    PopFont                         () final override;

            void                    PushFontStyle                   (vg::renderer::FontStyle _style) final override;
            void                    PopFontStyle                    () final override;

            CustomImGuiData &       GetCustomData                   (const core::string & _name) final override;

            gfx::ITexture *         GetPreviewTexture               (gfx::ITexture * _originalTex) final override;

            void                    RegisterFrameGraph              (const gfx::RenderPassContext & _renderPassContext, gfx::FrameGraph & _frameGraph);

        protected:
            void                    clearPreviewTextures            ();

            ImTextureID             getTextureID                    (const gfx::Texture * _texture);
            void                    releaseTextureID                (const gfx::Texture * _texture);
            void                    releaseUserDescriptors          ();

            bool                    createFont                      (Font _font, FontStyle _style);
            void                    updateFonts                     ();

            void                    resetGUITheme                   ();
            void                    setGUITheme_ImGui_Classic       ();
            void                    setGUITheme_ImGui_Dark          ();
            void                    setGUITheme_ImGui_Light         ();
            void                    setGUIThemeVimontGames_Grey     ();
            void                    setGUIThemeVimontGames_Dark     ();
            void                    onGUIThemeChanged               ();

            gfx::PixelFormat        getOutputPixelFormat            () const;
            void                    updateBackbufferFormat          ();

            #ifdef VG_VULKAN
            void                    createVulkanRenderPass          ();
            #endif

        private:
            static const core::uint             s_maxImGuiTexDisplayedPerFrame;
            static const Font                   s_defaultFont;
            static const FontStyle              s_defaultFontStyle;
            static const core::u8               s_defaultFontSize;

            gfx::BindlessTextureHandle          m_fontTexHandle;
            bool                                m_rebuildFontTex = false;

            Font                                m_currentFont;
            FontStyle                           m_currentFontStyle;
            core::u8                            m_currentFontSize;

            core::vector<BeginFrameCallback>    m_beginFrameCallbacks;

            #ifdef VG_VULKAN
            VkFormat                            m_vkRenderTargetFormat;
            VkDescriptorPool                    m_vkImguiDescriptorPool;
            VkRenderPass                        m_vkImguiRenderPass;
            VkSampler                           m_vkSampler;
            #endif

            struct DescriptorSetsFrameData
            {
                struct AllocData
                {
                    core::u32       m_refCount  = 0;
                    ImTextureID     m_id        = 0;
                };

                core::map<const gfx::Texture*, AllocData>  m_descriptorSetAllocs;
            };

            DescriptorSetsFrameData             m_descriptorSetsFrameData[2];

            ImVec4                              m_textColor;
            ImVec4                              m_rowColorEven;
            ImVec4                              m_rowColorOdd;
            ImVec4                              m_warningColor;
            ImVec4                              m_errorColor;
            ImVec4                              m_unsavedPropertyColor;
            ImVec4                              m_prefabOverridePropertyColor;

            struct ImFontInfo
            {
                bool     needed = false;
                bool     failed = false;
                ImFont * ptr    = nullptr;
            };
            ImFontInfo                          m_imGuiFont[core::enumCount<Font>()][core::enumCount<FontStyle>()];

            ImGuiSettings                       m_settingsHandler;
            core::unordered_map<TexturePreviewKey, TexturePreviewData, TexturePreviewKey::hash> m_previewTextures;
        };
    }
}