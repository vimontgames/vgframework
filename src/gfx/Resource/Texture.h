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
		public:
			VG_CLASS_DECL_PASSTHROUGH(Texture, ITexture);

			Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData);
			~Texture();

			core::u16										GetWidth				() const final override;
			core::u16										GetHeight				() const final override;
			gfx::PixelFormat								GetPixelFormat			() const final override;

			VG_INLINE const TextureDesc &					getTexDesc				() const;
			VG_INLINE const gfx::Resource &					getResource				() const;
			VG_INLINE gfx::Resource &						getResource				();
			VG_INLINE const gfx::BindlessTextureHandle		getTextureHandle		() const;
			VG_INLINE const gfx::BindlessTextureHandle		getDepthTextureHandle	() const;
			VG_INLINE const gfx::BindlessTextureHandle		getStencilTextureHandle	() const;
			VG_INLINE const gfx::BindlessRWTextureHandle	getRWTextureHandle		() const;

            static core::u32								getPixelFormatSize		(PixelFormat _format);
            static bool										isDepthStencilFormat	(PixelFormat _format);
            static core::u32								computeMaxMipmapCount	(const TextureDesc & _texDesc);
			
        protected:
			TextureDesc										m_texDesc;
            gfx::Resource									m_resource;
			union
			{
				gfx::BindlessTextureHandle					m_textureHandle;
				struct  
				{
					gfx::BindlessTextureHandle				m_depthTextureHandle;
					gfx::BindlessTextureHandle				m_stencilTextureHandle;
				};
			};
			
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
	public:
		VG_CLASS_DECL_PASSTHROUGH(Texture, VG_GFXAPI::Texture);

		Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
		~Texture();

        static Texture * createFromImporterData(const TextureImporterData & _data);
	};
}

#if VG_ENABLE_INLINE
#include "Texture.inl"
#endif