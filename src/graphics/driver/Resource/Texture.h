#pragma once

#include "Texture_consts.h"
#include "core/Object/Object.h"
#include "Resource.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class Texture : public core::Object
		{
		public:
            static core::u32 getPixelFormatSize(PixelFormat _format);

			Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
			~Texture();

			const TextureDesc & getTexDesc() const { return m_texDesc; }
            const driver::Resource & getResource() const { return m_resource; }
            const driver::BindlessTextureHandle getBindlessTextureHandle() const { return m_bindlessTextureHandle; }
			
        protected:
			const TextureDesc               m_texDesc;
            driver::Resource                m_resource;
            driver::BindlessTextureHandle   m_bindlessTextureHandle = driver::invalidBindlessTextureHandle;
		};
	}
}

#include VG_GFXAPI_HEADER(Texture)

namespace vg::graphics::driver
{
	class Texture : public VG_GFXAPI::Texture
	{
		using super = VG_GFXAPI::Texture;

	public:
		Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData = nullptr);
		~Texture();
	};
}