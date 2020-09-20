#pragma once

#include "Texture_consts.h"
#include "core/Object/Object.h"
#include "Resource.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class Texture : public core::Object
		{
		public:
			Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
			~Texture();

			const TextureDesc & getTexDesc() const { return m_texDesc; }
            const driver::Resource & getResource() const { return m_resource; }
			
        protected:
			const TextureDesc   m_texDesc;
            driver::Resource    m_resource;
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
		Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
		~Texture();
	};
}