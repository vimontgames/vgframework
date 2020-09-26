#pragma once

#include "graphics/driver/Resource/Texture.h"

namespace D3D12MA
{
    class Allocation;
}

namespace vg::graphics::driver::dx12
{
	class Texture : public base::Texture
	{
		using super = base::Texture;

	public:
        static DXGI_FORMAT                  getd3d12PixelFormat                 (PixelFormat _format);
        static D3D12_RESOURCE_DIMENSION     getd3d12ResourceDimension           (TextureType _texType);
        static D3D12_SRV_DIMENSION          getd3d12ShaderResourceViewDimension (TextureType _texType);
        static D3D12_RESOURCE_DESC          getd3d12ResourceDesc                (const TextureDesc & _texDesc);

        static PixelFormat                  getPixelFormat                      (DXGI_FORMAT _d3d12Format);
       
											Texture				                (const TextureDesc & _texDesc, const core::string & _name, void * _initData);
											~Texture			                ();

        //const D3D12_CPU_DESCRIPTOR_HANDLE & getd3d12SRVHandle                   () const;
		const D3D12_CPU_DESCRIPTOR_HANDLE & getd3d12RTVHandle	                () const;

	private:
        static const inline D3D12_CPU_DESCRIPTOR_HANDLE s_d3d12invalidSRVHandle = { (core::uint)-1 };
		static const inline D3D12_CPU_DESCRIPTOR_HANDLE s_d3d12invalidRTVHandle = { (core::uint)-1 };
        
        D3D12_CPU_DESCRIPTOR_HANDLE			m_d3d12RTVHandle = s_d3d12invalidRTVHandle;
	};
}