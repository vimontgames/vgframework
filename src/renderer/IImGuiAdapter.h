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
        class IImGuiAdapter
        {
        public:
            using BeginFrameCallback = void(*)(void);

            virtual             ~IImGuiAdapter          () = default;

            virtual bool        IsKeyboardFocused       () const = 0;
            virtual bool        IsMouseFocused          () const = 0;

            virtual ImTextureID GetTextureID            (const gfx::ITexture * _texture) const = 0;
            virtual void        ReleaseTextureID        (ImTextureID _texID) = 0;

            virtual void        AddBeginFrameCallback   (BeginFrameCallback _func) = 0;
        };
    }
}