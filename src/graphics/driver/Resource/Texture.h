#pragma once

#include "Resource.h"
#include "Texture_consts.h"
#include "graphics/driver/ITexture.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class Texture : public ITexture
		{
            using super = ITexture;

		public:
            static core::u32 getPixelFormatSize(PixelFormat _format);
            static bool isDepthStencilFormat(PixelFormat _format);
            static core::u32 computeMaxMipmapCount(const TextureDesc & _texDesc);

			Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData);
			~Texture();

			const TextureDesc & getTexDesc() const { return m_texDesc; }
            const driver::Resource & getResource() const { return m_resource; }
            driver::Resource & getResource() { return m_resource; }
            const driver::BindlessTextureSrvHandle getBindlessSRVHandle() const { return m_bindlessSRVHandle; }
			
        protected:
			const TextureDesc                   m_texDesc;
            driver::Resource                    m_resource;
            driver::BindlessTextureSrvHandle    m_bindlessSRVHandle;
		};
	}
}

#include VG_GFXAPI_HEADER(Texture)

namespace vg::graphics::driver
{
    class TextureImporterData;

	class Texture : public VG_GFXAPI::Texture
	{
		using super = VG_GFXAPI::Texture;

	public:
        const char * getClassName() const final { return "Texture"; }

		Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
		~Texture();

        static Texture * createFromImporterData(const TextureImporterData & _data);
	};
}