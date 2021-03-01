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
            static bool isDepthStencilFormat(PixelFormat _format);

			Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
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
	class Texture : public VG_GFXAPI::Texture
	{
		using super = VG_GFXAPI::Texture;

	public:
		Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData = nullptr, ReservedSlot _reservedSlot = ReservedSlot::None);
		~Texture();
	};
}