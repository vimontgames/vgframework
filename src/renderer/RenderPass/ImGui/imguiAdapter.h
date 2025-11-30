#pragma once

#include "renderer/IImGuiAdapter.h"
#include "renderer/RenderPass/Render2D/Preview/Texture/TexturePreview.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"
#include "ImGuiSettings.h"
#include "ImguiFont.h"

typedef vg::core::u64 ImTextureID;

namespace vg::gfx
{
    class Device;
    class Texture;
    class CommandList;
}

namespace vg::renderer::base
{
    class ImGuiAdapter : public IImGuiAdapter
    {
    public:
    protected:
        gfx::PixelFormat getOutputPixelFormat() const;

    protected:
        static const core::uint s_maxImGuiTexDisplayedPerFrame;
    };
}

#ifdef VG_DX12
#include "dx12/ImGuiAdapter_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/ImGuiAdapter_Vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::renderer
{
    class ImGuiAdapter final : public vg::renderer::VG_GFXAPI::ImGuiAdapter
    {
    public:
        using super = vg::renderer::VG_GFXAPI::ImGuiAdapter;

        ImGuiAdapter(core::WinHandle _winHandle, gfx::Device & _device);
        ~ImGuiAdapter();

        void                    init                                ();

        void                    SetGUITheme                         (Theme _theme) final override;
        bool                    IsCurrentThemeDark                  () const final override;

        const ImVec4 &          GetTextColor                        () const final override { return m_textColor; }
        const ImVec4 &          GetTransientPropertyColor           () const final override { return m_unsavedPropertyColor; }
        const ImVec4 &          GetTransientReadOnlyPropertyColor   () const final override { return m_unsavedReadOnlyPropertyColor; }
        const ImVec4 &          GetPrefabOverridePropertyColor      () const final override { return m_prefabOverridePropertyColor; }

        const ImVec4 &          GetRowColorEven                     () const final override { return m_rowColorEven;}
        const ImVec4 &          GetRowColorOdd                      () const final override { return m_rowColorOdd; }

        const ImVec4 &          GetWarningColor                     () const final override { return m_warningColor; }
        const ImVec4 &          GetErrorColor                       () const final override { return m_errorColor; }

        bool                    IsKeyboardFocused                   () const final override;
        bool                    IsMouseFocused                      () const final override;

        ImTextureID             GetTextureID                        (const gfx::ITexture * _texture) final override;
        void                    ReleaseTextureID                    (const gfx::ITexture * _texture) final override;

        void                    beginFrame                          (gfx::CommandList * _cmdList);
        void                    render                              (gfx::CommandList * _cmdList);

        void                    AddBeginFrameCallback               (BeginFrameCallback _func);
        ImFont *                GetFont                             (Font _font, FontStyle _style) final override;
        const char *            GetFontPath                         (Font _font, FontStyle _style) const final override;

        void                    PushDefaultFont                     () final override;
        void                    PushFont                            (vg::renderer::Font _font) final override;
        void                    PushFont                            (vg::renderer::Font _font, vg::renderer::FontStyle _style) final override;
        void                    PushFont                            (vg::renderer::Font _font, vg::renderer::FontStyle _style, vg::renderer::FontSize _size) final override;
        void                    PopFont                             () final override;

        void                    PushFontStyle                       (vg::renderer::FontStyle _style) final override;
        void                    PopFontStyle                        () final override;

        void                    PushFontSize                        (vg::renderer::FontSize _size) final override;
        void                    PopFontSize                         () final override;

        CustomImGuiData &       GetCustomData                       (const core::string & _name) final override;

        gfx::ITexture *         GetPreviewTexture                   (gfx::ITexture * _originalTex) final override;

        void                    RegisterFrameGraph                  (const gfx::RenderPassContext & _renderPassContext, gfx::FrameGraph & _frameGraph);

    protected:
        void                    clearPreviewTextures                ();

        ImTextureID             getTextureID                        (const gfx::Texture * _texture);
        void                    releaseTextureID                    (const gfx::Texture * _texture);
        void                    releaseUserDescriptors              ();

        ImFontInfo &            getOrCreateFontInfo                 (Font _font, FontStyle _style);
        bool                    createFont                          (Font _font, FontStyle _style);
        void                    updateFonts                         ();

        void                    resetGUITheme                       ();
        void                    setGUITheme_ImGui_Classic           ();
        void                    setGUITheme_ImGui_Dark              ();
        void                    setGUITheme_ImGui_Light             ();
        void                    setGUIThemeVimontGames_Grey         ();
        void                    setGUIThemeVimontGames_Dark         ();
        void                    onGUIThemeChanged                   ();

    private:
        static const Font                   s_defaultFont;
        static const FontStyle              s_defaultFontStyle;
        static const FontSize               s_defaultFontSize;

        Font                                m_currentFont;
        FontStyle                           m_currentFontStyle;
        FontSize                            m_currentFontSize;

        core::vector<BeginFrameCallback>    m_beginFrameCallbacks;

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
        ImVec4                              m_unsavedReadOnlyPropertyColor;
        ImVec4                              m_prefabOverridePropertyColor;
        ImGuiSettings                       m_settingsHandler;
        core::unordered_map<ImFontKey, ImFontInfo, ImFontKey::hash> m_imGuiFont;
        core::unordered_map<TexturePreviewKey, TexturePreviewData, TexturePreviewKey::hash> m_previewTextures;
        core::vector<core::uint>            m_fontSizeStack;
    };
}