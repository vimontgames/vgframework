#include "ImGui.h"
#include "imguiAdapter.h"
#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/CommandQueue/CommandQueue.h"
#include "gfx/Resource/Texture.h"
#include "gfx/BindlessTable/BindlessTable.h"
#include "editor/Editor_Consts.h"
#include "ImGuiFileDialog/ImGuiFileDialog.cpp"
#include "IconFont/IconsFontAwesome6.h"
#include "renderer/Model/Material/Material_Consts.h"
#include "renderer/RenderPass/Render2D/Preview/Texture/TexturePreviewPass.h"

#ifdef VG_DX12
#include "dx12/ImGuiAdapter_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/ImGuiAdapter_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer::base
{
    //--------------------------------------------------------------------------------------
    // Returns the pixel format used for ImGui rendering
    // It should always be the same format used in HDROutput pass!
    //--------------------------------------------------------------------------------------
    gfx::PixelFormat ImGuiAdapter::getOutputPixelFormat() const
    {
        gfx::Device * device = Device::get();
        const PixelFormat backbufferFormat = device->getBackbufferFormat();
        const PixelFormat hdrOutputFormat = gfx::PixelFormat::R16G16B16A16_float;

        auto * renderer = Renderer::get();

        if (HDR::None != renderer->GetHDR())
           return hdrOutputFormat;
        else
            return backbufferFormat;
    }
}

namespace vg::renderer
{
    namespace base
    {
        const core::uint ImGuiAdapter::s_maxImGuiTexDisplayedPerFrame = 64;
    }

    const Font      ImGuiAdapter::s_defaultFont      = Font::UbuntuMono;
    const FontStyle ImGuiAdapter::s_defaultFontStyle = FontStyle::Regular;
    const FontSize  ImGuiAdapter::s_defaultFontSize  = (FontSize)round(style::font::DefaultFontHeight);

    //--------------------------------------------------------------------------------------
    // TODO: move themes to editor?
    //--------------------------------------------------------------------------------------
    ImGuiAdapter::ImGuiAdapter(WinHandle _winHandle, Device & _device) :
        m_currentFont(s_defaultFont),
        m_currentFontStyle(s_defaultFontStyle),
        m_currentFontSize(s_defaultFontSize)
    {        
        VG_PROFILE_CPU("ImGui");

        ImGui::CreateContext();
        ImGuiIO & io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        m_settingsHandler.init();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingTransparentPayload = true;

        #ifdef _WIN32
        ImGui_ImplWin32_Init(_winHandle);
        #endif

        io.BackendFlags &= ~ImGuiBackendFlags_PlatformHasViewports; // Not supported by 1.92.5 Vulkan backend        

        init();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::init()
    {
        super::init();
    }

    //--------------------------------------------------------------------------------------
    const char * ImGuiAdapter::GetFontPath(Font _font, FontStyle _style) const
    {
        switch (_font)
        {
            case Font::Rowndies:
            {
                switch (_style)
                {
                    case FontStyle::Regular:
                        return "Rowdies/Rowdies-Regular.ttf";
                    case FontStyle::Bold:
                        return "Rowdies/Rowdies-Bold.ttf";
                    case FontStyle::Light:
                        return "Rowdies/Rowdies-Light.ttf";
                };
            }
            break;

            case Font::RubikMonoOne:
            {
                switch (_style)
                {
                    case FontStyle::Regular:
                        return "RubikMonoOne/RubikMonoOne-Regular.ttf";
                };
            }
            break;    

            case Font::UbuntuMono:
            {
                switch (_style)
                {
                case FontStyle::Regular:
                    return "ubuntu/UbuntuMono-R.ttf";
                case FontStyle::Bold:
                    return "ubuntu/UbuntuMono-B.ttf";
                case FontStyle::Italic:
                    return "ubuntu/UbuntuMono-RI.ttf";
                };
            }
            break;
        }

        return nullptr;
    };

    //--------------------------------------------------------------------------------------
    ImFontInfo & ImGuiAdapter::getOrCreateFontInfo(Font _font, FontStyle _style)
    {
        ImFontKey key(_font, _style);
        auto it = m_imGuiFont.find(key);
        if (it != m_imGuiFont.end())
        {
            return it->second;
        }
        else
        {
            ImFontInfo info;
            return m_imGuiFont.insert({ key, info }).first->second;
        }
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiAdapter::createFont(Font _font, FontStyle _style)
    {
        VG_PROFILE_CPU("createFont");

        const auto fontPath = GetFontPath(_font, _style);
        auto & slot = getOrCreateFontInfo(_font, _style);

        if (fontPath)
        {
            string fontFullPath = fmt::sprintf("data/Engine/Fonts/%s", fontPath);

            ImGuiIO & io = ImGui::GetIO();
            io.Fonts->AddFontFromFileTTF(fontFullPath.c_str());

            // merge in icons from Font Awesome
            static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            slot.ptr = io.Fonts->AddFontFromFileTTF("data/Engine/Fonts/Font-Awesome-6.x/" FONT_ICON_FILE_NAME_FAS, 0, &icons_config, icons_ranges);
        }

        return nullptr != slot.ptr;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::updateFonts()
    {
        for (auto & pair : m_imGuiFont)
        {
            auto & key = pair.first;
            auto & slot = pair.second;

            if (slot.ptr == nullptr && slot.needed && !slot.failed)
            {
                const char * path = GetFontPath(key.font, key.style);

                if (createFont(key.font, key.style))
                {
                    VG_INFO("[UI] Created font texture {%s,%s} from file \"%s\"", asString(key.font).c_str(), asString(key.style).c_str(), path);
                }
                else
                {
                    slot.failed = true;

                    if (path)
                        VG_ERROR("[UI] Could not create font texture {%s,%s} from file \"%s\"", asString(key.font).c_str(), asString(key.style).c_str(), path);
                    else
                        VG_ERROR("[UI] Could not create font texture {%s,%s}", asString(key.font).c_str(), asString(key.style).c_str());
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    ImGuiAdapter::~ImGuiAdapter()
    {
        gfx::Device * device = Device::get();
        device->waitGPUIdle();

        clearPreviewTextures();

        super::deinit();

        #ifdef _WIN32
        ImGui_ImplWin32_Shutdown();
        #endif //_WIN32

        ImGui::DestroyContext();
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiAdapter::IsKeyboardFocused() const
    {
        const ImGuiIO & io = ImGui::GetIO();
        if (io.WantCaptureKeyboard)
            return true;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiAdapter::IsMouseFocused() const
    {
        const ImGuiIO & io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            return true;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    ImTextureID ImGuiAdapter::GetTextureID(const gfx::ITexture * _texture)
    {
        return getTextureID((Texture *)_texture);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::ReleaseTextureID(const gfx::ITexture * _texture)
    {
        releaseTextureID((Texture *)_texture);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::beginFrame(gfx::CommandList * _cmdList)
    {
        VG_PROFILE_CPU("beginFrame");

        gfx::Device * device = Device::get();

        updateFonts();

        super::updateBackbufferFormat();

        super::beginFrame();

        releaseUserDescriptors();

        #ifdef _WIN32
        ImGui_ImplWin32_NewFrame();
        #endif       

        ImGui::NewFrame();

        for (auto & cb : m_beginFrameCallbacks)
            cb();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::releaseUserDescriptors()
    {
        DescriptorSetsFrameData & descriptorSetsFrameData = m_descriptorSetsFrameData[1];

        for (const auto & data : descriptorSetsFrameData.m_descriptorSetAllocs)
        {
            VG_ASSERT(data.second.m_refCount == 0, "ImTextureID %u from Texture \"%s\" still has a RefCount of %u and was not released", data.second.m_id, data.first->GetName().c_str(), data.second.m_refCount);
            
            if (0 == data.second.m_refCount)
                super::removeTexture(data.second.m_id);
        }

        descriptorSetsFrameData.m_descriptorSetAllocs.clear();
        m_descriptorSetsFrameData[1] = std::move(m_descriptorSetsFrameData[0]);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::AddBeginFrameCallback(BeginFrameCallback _func)
    {
        m_beginFrameCallbacks.push_back(_func);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::render(gfx::CommandList * _cmdList)
    {
        VG_PROFILE_CPU("render");

        super::render(_cmdList);

        // Clear gfx state cache after rendering ImGui because it could have changed any gfx state
        _cmdList->clearStateCache();
    }

    //--------------------------------------------------------------------------------------
    ImTextureID ImGuiAdapter::getTextureID(const gfx::Texture * _texture)
    {
        auto device = gfx::Device::get();

        ImTextureID id;

        auto & descriptorSetsFrameData = m_descriptorSetsFrameData[0];
        auto it = descriptorSetsFrameData.m_descriptorSetAllocs.find(_texture);

        if (descriptorSetsFrameData.m_descriptorSetAllocs.end() == it)
        {
            id = super::addTexture(_texture);

            DescriptorSetsFrameData::AllocData allocData;
            allocData.m_id = id;

            descriptorSetsFrameData.m_descriptorSetAllocs.insert(std::pair(_texture, allocData));
            it = descriptorSetsFrameData.m_descriptorSetAllocs.find(_texture);
        }
        
        it->second.m_refCount++;

        return it->second.m_id;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::releaseTextureID(const gfx::Texture * _texture)
    {
        auto & descriptorSetsFrameData = m_descriptorSetsFrameData[0];

        auto it = descriptorSetsFrameData.m_descriptorSetAllocs.find(_texture);
        VG_ASSERT(descriptorSetsFrameData.m_descriptorSetAllocs.end() != it);

        it->second.m_refCount--;

        VG_ASSERT(descriptorSetsFrameData.m_descriptorSetAllocs.size() <= s_maxImGuiTexDisplayedPerFrame);
    }

    //--------------------------------------------------------------------------------------
    ImFont * ImGuiAdapter::GetFont(Font _font, FontStyle _style)
    {
        auto & info = getOrCreateFontInfo(_font,_style);

        if (!info.ptr)
        {
            info.needed = true; // Rebuild font next frame

            // Fallback to regular style if not found
            if (_style != FontStyle::Regular)
                return GetFont(_font, FontStyle::Regular);
        }

        return info.ptr;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PushDefaultFont()
    {
        ImGui::PushFont(GetFont(s_defaultFont, s_defaultFontStyle), s_defaultFontSize);
        m_currentFont = s_defaultFont;
        m_currentFontStyle = s_defaultFontStyle;
        m_currentFontSize = s_defaultFontSize;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PushFont(vg::renderer::Font _font)
    {
        ImGui::PushFont(GetFont(_font, m_currentFontStyle), m_currentFontSize);
        m_currentFont = _font;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PushFont(vg::renderer::Font _font, vg::renderer::FontStyle _style)
    {
        auto * font = GetFont(_font, _style);
        ImGui::PushFont(font);
        m_currentFont = _font;
        m_currentFontStyle = _style;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PushFont(vg::renderer::Font _font, vg::renderer::FontStyle _style, vg::renderer::FontSize _size)
    {
        auto * font = GetFont(_font, _style);
        ImGui::PushFont(font, _size);
        m_currentFont = _font;
        m_currentFontStyle = _style;
        m_currentFontSize = _size;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PopFont()
    {
        ImGui::PopFont();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PushFontStyle(vg::renderer::FontStyle _style)
    {
        ImGui::PushFont(GetFont(m_currentFont, _style), m_currentFontSize);
        m_currentFontStyle = _style;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PopFontStyle()
    {
        ImGui::PopFont();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PushFontSize(vg::renderer::FontSize _size)
    {
        m_fontSizeStack.push_back(m_currentFontSize);
        m_currentFontSize = _size;
        ImGui::PushFont(GetFont(m_currentFont, m_currentFontStyle), m_currentFontSize);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::PopFontSize()
    {
        VG_ASSERT(m_fontSizeStack.size() > 0);
        if (m_fontSizeStack.size() > 0)
        {
            m_currentFontSize = m_fontSizeStack.back();
            m_fontSizeStack.pop_back();
        }
        ImGui::PopFont();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::SetGUITheme(Theme _theme)
    {
        resetGUITheme();

        switch (_theme)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_theme);
                break;

            case Theme::ImGui_Classic:
                setGUITheme_ImGui_Classic();
                break;

            case Theme::ImGui_Dark:
                setGUITheme_ImGui_Dark();
                break;

            case Theme::ImGui_Light:
                setGUITheme_ImGui_Light();
                break;

            case Theme::VimontGames_Grey:
                setGUIThemeVimontGames_Grey();
                break;

            case Theme::VimontGames_Dark:
                setGUIThemeVimontGames_Dark();
                break;
        }

        onGUIThemeChanged();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::resetGUITheme()
    {
        ImGui::GetStyle() = ImGuiStyle();

        ImGuiStyle & style = ImGui::GetStyle();
        style.TabBarOverlineSize = 0;
        style.DockingSeparatorSize = 0;
        style.DisabledAlpha = 0.8f;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::setGUITheme_ImGui_Classic()
    {
        ImGui::StyleColorsClassic();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::setGUITheme_ImGui_Dark()
    {
        ImGui::StyleColorsDark();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::setGUITheme_ImGui_Light()
    {
        ImGui::StyleColorsLight();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::setGUIThemeVimontGames_Grey()
    {
        ImGuiStyle & style = ImGui::GetStyle();
        ImVec4 * colors = style.Colors;

        float rounding = 8.0f;

        style.PopupRounding = rounding;
        style.WindowPadding = ImVec2(4, 4);
        style.FramePadding = ImVec2(4, 4);
        style.ItemSpacing = ImVec2(4, 4);
        style.ItemInnerSpacing = ImVec2(4, 4);
        style.ScrollbarSize = 16;
        style.IndentSpacing = 16;
        style.WindowBorderSize = 0;
        style.ChildBorderSize = 0;
        style.PopupBorderSize = 0;
        style.FrameBorderSize = 0;
        style.WindowRounding = rounding;
        style.ChildRounding = rounding;
        style.FrameRounding = rounding;
        style.ScrollbarRounding = rounding;
        style.GrabRounding = rounding;
        style.Alpha = 0.9f;

        #ifdef IMGUI_HAS_DOCK 
        style.TabBorderSize = 0;
        style.TabRounding = rounding;
        #endif

        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
        colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.13f, 0.15f, 0.50f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.231f, 0.310f, 0.447f, 1.000f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.231f, 0.310f, 0.447f, 1.000f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.23f, 0.34f, 0.54f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.23f, 0.34f, 0.54f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.23f, 0.34f, 0.54f, 0.77f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.33f, 0.42f, 0.60f, 0.77f);
        colors[ImGuiCol_Tab] = ImVec4(0.33f, 0.42f, 0.60f, 0.50f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_TabSelected] = ImVec4(0.33f, 0.42f, 0.60f, 1.00f);
        colors[ImGuiCol_TabDimmed] = ImVec4(0.54f, 0.54f, 0.54f, 0.19f);
        colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.54f, 0.54f, 0.54f, 0.38f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.20f, 0.29f, 0.43f, 0.77f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::setGUIThemeVimontGames_Dark()
    {
        ImGuiStyle & style = ImGui::GetStyle();
        ImVec4 * colors = style.Colors;

        float rounding = 8.0f;

        style.PopupRounding = rounding;
        style.WindowPadding = ImVec2(4, 4);
        style.FramePadding = ImVec2(2, 4);
        style.ItemSpacing = ImVec2(2, 4);
        style.ItemInnerSpacing = ImVec2(2, 4);
        style.ScrollbarSize = 16;
        style.IndentSpacing = 8;
        style.WindowBorderSize = 0;
        style.ChildBorderSize = 0;
        style.PopupBorderSize = 0;
        style.FrameBorderSize = 0;
        style.WindowRounding = rounding;
        style.ChildRounding = rounding;
        style.FrameRounding = rounding;
        style.ScrollbarRounding = rounding;
        style.GrabRounding = rounding;
        style.Alpha = 1.0f;

        #ifdef IMGUI_HAS_DOCK 
        style.TabBorderSize = 0;
        style.TabRounding = rounding;
        style.TabBarBorderSize = 0;
        #endif 

        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
        colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_Border] =  ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.21f, 0.24f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.32f, 0.59f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.18f, 0.21f, 0.24f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.07f, 0.32f, 0.59f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.07f, 0.32f, 0.59f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.07f, 0.32f, 0.59f, 1.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.07f, 0.32f, 0.59f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_TabSelected] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_TabDimmed] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.01f, 0.40f, 0.84f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.78f, 0.00f, 1.00f);
        colors[ImGuiCol_NavCursor] = ImVec4(0.91f, 0.72f, 0.08f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiAdapter::IsCurrentThemeDark() const
    {
        ImVec4 bgColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

        float grey = (bgColor.x + bgColor.y + bgColor.z) / 3.0f;
        return grey < 0.5f;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::onGUIThemeChanged()
    {
        m_textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        m_unsavedPropertyColor = ImGui::GetStyleColorVec4(ImGuiCol_TextLink);
        m_prefabOverridePropertyColor = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);

        const auto disabledColor = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
        m_unsavedReadOnlyPropertyColor = ImVec4(
            lerp(m_unsavedPropertyColor.x, disabledColor.x, 0.5f),
            lerp(m_unsavedPropertyColor.y, disabledColor.y, 0.5f),
            lerp(m_unsavedPropertyColor.z, disabledColor.z, 0.5f),
            lerp(m_unsavedPropertyColor.w, disabledColor.w, 0.5f)
        );

        // update alternating row colors
        ImVec4 bgColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        float d1, d2;
        if (IsCurrentThemeDark())
        {
            d1 = 0.1f;
            d2 = 0.2f;
        }
        else
        {
            d1 = -0.1f;
            d2 = -0.2f;
        }
        m_rowColorEven = ImVec4(bgColor.x + d1, bgColor.y + d1, bgColor.z + d1, bgColor.w * 0.35f);
        m_rowColorOdd = ImVec4(bgColor.x + d2, bgColor.y + d2, bgColor.z + d2, bgColor.w * 0.35f);

        // update warning and error colors
        if (IsCurrentThemeDark())
        {
            m_warningColor = ImVec4(0.9f, 0.9f, 0.3f, 1.0f);
            m_errorColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
        }
        else
        {
            m_warningColor = ImVec4(0.5f, 0.4f, 0.0f, 1.0f);
            m_errorColor = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
        }

        // TODO : DLL context!
        const auto textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImGuiFileDialog * fileDialog = ImGuiFileDialog::Instance();

        fileDialog->SetFileStyle(IGFD_FileStyleByExtention, ".world", textColor, editor::style::icon::World);
        fileDialog->SetFileStyle(IGFD_FileStyleByExtention, ".scene", textColor, editor::style::icon::Scene);
        fileDialog->SetFileStyle(IGFD_FileStyleByExtention, ".prefab", textColor, editor::style::icon::Prefab);
        fileDialog->SetFileStyle(IGFD_FileStyleByExtention, ".fbx", textColor, editor::style::icon::Mesh);

        fileDialog->SetFileStyle(IGFD_FileStyleByTypeDir, "", textColor, editor::style::icon::Folder);
        fileDialog->SetFileStyle(IGFD_FileStyleByTypeFile, "", textColor, editor::style::icon::File);
    }

    //--------------------------------------------------------------------------------------
    CustomImGuiData & ImGuiAdapter::GetCustomData(const core::string & _name)
    {
        return m_settingsHandler.getCustomData(_name);
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * ImGuiAdapter::GetPreviewTexture(ITexture * _originalTex)
    {
        const TexturePreviewKey key = { (Texture *)_originalTex };
        auto it = m_previewTextures.find(key);
        if (m_previewTextures.end() == it)
        {
            gfx::Device * device = Device::get();
            gfx::TextureDesc previewDesc = ((Texture*)_originalTex)->getTexDesc();
            previewDesc.type = gfx::TextureType::Texture2D;
            previewDesc.slices = 1;
            previewDesc.mipmaps = 1;
            previewDesc.flags |= gfx::TextureFlags::RenderTarget;
            previewDesc.format = gfx::PixelFormat::R8G8B8A8_unorm;

            TexturePreviewData data;
            data.m_previewTex = device->createTexture(previewDesc, "[Preview]" + _originalTex->GetName());
            data.m_previewPass = new TexturePreviewPass("TexturePreview");
        
            m_previewTextures.insert({ key, data });
            return nullptr;
        }
        else
        {
            it->second.m_dirty = true; // redraw every frame
            return it->second.m_previewTex;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::RegisterFrameGraph(const gfx::RenderPassContext & _renderPassContext, FrameGraph & _frameGraph)
    {
        uint index = 0;
        for (auto & pair : m_previewTextures)
        {
            if (pair.second.m_dirty)
            {
                string name = fmt::sprintf("TexturePreview #%u", index);

                const auto previewTex = pair.second.m_previewTex;
                const auto previewPass = pair.second.m_previewPass;

                previewPass->setSourceTexture(pair.first.m_texture, index, pair.second.m_firstUse);

                _frameGraph.importRenderTarget(name, previewTex, float4(0, 0, 0, 1), FrameGraphResource::InitState::Clear);
                _frameGraph.addUserPass(_renderPassContext, previewPass, name);

                index++;

                pair.second.m_dirty = false;
                pair.second.m_firstUse = false;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiAdapter::clearPreviewTextures()
    {
        for (auto & pair : m_previewTextures)
        {
            VG_SAFE_RELEASE(pair.second.m_previewTex);
            VG_SAFE_RELEASE(pair.second.m_previewPass);
        }
        m_previewTextures.clear();
    }
}
