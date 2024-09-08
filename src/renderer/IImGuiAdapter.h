#pragma once

typedef void * ImTextureID;
struct ImVec4;
struct ImFont;

namespace ImGui
{
    vg_enum_class(Font, vg::core::u8,
        UbuntuMono
    );

    vg_enum_class(Style, vg::core::u8,
        Regular = 0,
        Bold,
        Italic
    );

    vg_enum_class(Theme , vg::core::u8,
        ImGui_Classic,
        ImGui_Dark,
        ImGui_Light,
        VimontGames_Grey,
        VimontGames_Dark
    );
}

namespace vg
{
    namespace gfx
    {
        class ITexture;
    }

    namespace renderer
    {
        class IImGuiAdapter
        {
        public:
            using BeginFrameCallback = void(*)(void);

            virtual                 ~IImGuiAdapter          () = default;

            virtual void            SetGUITheme             (ImGui::Theme _theme) = 0;
            virtual bool            IsCurrentThemeDark      () const = 0;
            virtual const ImVec4 &  GetRowColorEven         () const = 0;
            virtual const ImVec4 &  GetRowColorOdd          () const = 0;

            virtual const ImVec4 &  GetWarningColor         () const = 0;
            virtual const ImVec4 &  GetErrorColor           () const = 0;

            virtual bool            IsKeyboardFocused       () const = 0;
            virtual bool            IsMouseFocused          () const = 0;

            virtual ImTextureID     GetTextureID            (const gfx::ITexture * _texture) = 0;
            virtual void            ReleaseTextureID        (const gfx::ITexture * _texture) = 0;

            virtual void            AddBeginFrameCallback   (BeginFrameCallback _func) = 0;
            virtual ImFont *        GetFont                 (ImGui::Font _font, ImGui::Style _style = ImGui::Style::Regular) const = 0;
        };
    }
}