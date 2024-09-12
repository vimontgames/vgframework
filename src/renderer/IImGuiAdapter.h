#pragma once

#include "ImGui_consts.h"

typedef void * ImTextureID;
struct ImVec4;
struct ImFont;

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

            virtual void            SetGUITheme             (Theme _theme) = 0;
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
            virtual ImFont *        GetFont                 (Font _font, Style _style = Style::Regular) = 0;

            virtual void            PushFont                (vg::renderer::Font _font) = 0;
            virtual void            PushFont                (vg::renderer::Font _font, vg::renderer::Style _style) = 0;
            virtual void            PopFont                 () = 0;

            virtual void            PushStyle               (vg::renderer::Style _style) = 0;
            virtual void            PopStyle                () = 0;
        };
    }
}