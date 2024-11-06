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
        struct CustomImGuiData
        {
            bool isOpen;
        };

        class IImGuiAdapter
        {
        public:
            using BeginFrameCallback = void(*)(void);

            virtual                     ~IImGuiAdapter                  () = default;

            virtual void                SetGUITheme                     (Theme _theme) = 0;
            virtual bool                IsCurrentThemeDark              () const = 0;

            virtual const ImVec4 &      GetTextColor                    () const = 0;
            virtual const ImVec4 &      GetUnsavedPropertyColor         () const = 0;
            virtual const ImVec4 &      GetPrefabOverridePropertyColor  () const = 0;

            virtual const ImVec4 &      GetRowColorEven                 () const = 0;
            virtual const ImVec4 &      GetRowColorOdd                  () const = 0;

            virtual const ImVec4 &      GetWarningColor                 () const = 0;
            virtual const ImVec4 &      GetErrorColor                   () const = 0;

            virtual bool                IsKeyboardFocused               () const = 0;
            virtual bool                IsMouseFocused                  () const = 0;

            virtual ImTextureID         GetTextureID                    (const gfx::ITexture * _texture) = 0;
            virtual void                ReleaseTextureID                (const gfx::ITexture * _texture) = 0;

            virtual void                AddBeginFrameCallback           (BeginFrameCallback _func) = 0;
            virtual ImFont *            GetFont                         (Font _font, FontStyle _style = FontStyle::Regular) = 0;
            virtual const char *        GetFontPath                     (Font _font, FontStyle _style) const = 0;

            virtual void                PushDefaultFont                 () = 0;
            virtual void                PushFont                        (vg::renderer::Font _font) = 0;
            virtual void                PushFont                        (vg::renderer::Font _font, vg::renderer::FontStyle _style) = 0;
            virtual void                PopFont                         () = 0;

            virtual void                PushFontStyle                   (vg::renderer::FontStyle _style) = 0;
            virtual void                PopFontStyle                    () = 0;

            virtual CustomImGuiData &   GetCustomData                   (const core::string & _name) = 0;

            virtual gfx::ITexture *     GetPreviewTexture               (gfx::ITexture * _originalTex) = 0;
        };
    }
}