#pragma once

#include "Resource.h"
#include "gfx/ITexture.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::gfx
{
	namespace base
	{
		class Texture : public ITexture
		{
            using super = ITexture;

		public:
			Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData);
			~Texture();

            virtual core::u16								GetWidth				() const final override;
            virtual core::u16								GetHeight				() const final override;

			VG_INLINE const TextureDesc &					getTexDesc				() const;
			VG_INLINE const gfx::Resource &					getResource				() const;
			VG_INLINE gfx::Resource &						getResource				();
			VG_INLINE const gfx::BindlessTextureHandle		getTextureHandle		() const;
			VG_INLINE const gfx::BindlessRWTextureHandle	getRWTextureHandle		() const;

            static core::u32								getPixelFormatSize		(PixelFormat _format);
            static bool										isDepthStencilFormat	(PixelFormat _format);
            static core::u32								computeMaxMipmapCount	(const TextureDesc & _texDesc);
			
        protected:
			const TextureDesc								m_texDesc;
            gfx::Resource									m_resource;
            gfx::BindlessTextureHandle						m_textureHandle;
			gfx::BindlessRWTextureHandle					m_rwTextureHandle;
		};
	}
}

#include VG_GFXAPI_HEADER(Texture)

namespace vg::gfx
{
    class TextureImporterData;

	class Texture : public VG_GFXAPI::Texture
	{
		using super = VG_GFXAPI::Texture;

	public:
        const char *    getClassName() const final { return "Texture"; }
        static bool     registerClass(core::IFactory & _factory);
        static bool     registerProperties(core::IClassDesc & _desc);

		Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
		~Texture();

        static Texture * createFromImporterData(const TextureImporterData & _data);
	};
}

#if VG_ENABLE_INLINE
#include "Texture.inl"
#endif