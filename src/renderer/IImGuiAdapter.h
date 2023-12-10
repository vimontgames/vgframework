#pragma once

typedef void * ImTextureID;

namespace vg
{
    namespace gfx
    {
        class ITexture;
    }

    namespace renderer
    {
        enum class GUITheme : core::u8
        {
            ImGui_Classic,
            ImGui_Dark,
            ImGui_Light,
            VG_Dark
        };

        class IImGuiAdapter
        {
        public:
            using BeginFrameCallback = void(*)(void);

            virtual             ~IImGuiAdapter          () = default;

            virtual void        SetGUITheme             (GUITheme _theme) = 0;

            virtual bool        IsKeyboardFocused       () const = 0;
            virtual bool        IsMouseFocused          () const = 0;

            virtual ImTextureID GetTextureID            (const gfx::ITexture * _texture) const = 0;
            virtual void        ReleaseTextureID        (ImTextureID _texID) = 0;

            virtual void        AddBeginFrameCallback   (BeginFrameCallback _func) = 0;
        };
    }
}