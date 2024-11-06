#pragma once

namespace vg::renderer
{
    class TexturePreviewPass;

    struct TexturePreviewKey
    {
        gfx::Texture * m_texture;
        //gfx::TextureDesc m_desc;

        bool operator ==(const TexturePreviewKey & _other) const { return m_texture == _other.m_texture /* && m_desc == _other.m_desc*/; }

        struct hash
        {
            inline core::size_t operator() (const TexturePreviewKey & _this) const
            {
                return (size_t)_this.m_texture;// ^ std::hash<gfx::TextureDesc>()(_this.m_desc);
            }
        };
    };

    struct TexturePreviewData
    {
        bool                    m_dirty = true;
        bool                    m_firstUse = true;
        gfx::Texture *          m_previewTex = nullptr;
        TexturePreviewPass *    m_previewPass = nullptr;
    };
}