#pragma once

#include "gfx/Resource/Texture.h"

namespace D3D12MA
{
    class Allocation;
}

namespace vg::gfx
{
    struct SubResourceData
    {
        core::size_t offset = 0;    // Sub-Resource offset used for upload (TODO: should be shared with Vulkan version)
    };
}

namespace vg::gfx::dx12
{
	class Texture : public base::Texture
	{
    public:
        VG_CLASS_DECL_PASSTHROUGH(Texture, base::Texture);

        static DXGI_FORMAT                              getd3d12ResourceFormat              (PixelFormat _format);
        static DXGI_FORMAT                              getd3d12SurfaceFormat               (PixelFormat _format);
        static DXGI_FORMAT                              getd3d12ShaderResourceViewFormat    (PixelFormat _format, bool _stencil = false);
        static D3D12_RESOURCE_DIMENSION                 getd3d12ResourceDimension           (TextureType _texType);
        static D3D12_SRV_DIMENSION                      getd3d12ShaderResourceViewDimension (TextureType _texType);
        static D3D12_RTV_DIMENSION                      getd3d12RenderTargetViewDimension   (TextureType _texType);
        static D3D12_DSV_DIMENSION                      getd3d12DepthStencilViewDimension   (TextureType _texType);
        static D3D12_RESOURCE_DESC                      getd3d12ResourceDesc                (const TextureDesc & _texDesc);

        static PixelFormat                              getPixelFormat                      (DXGI_FORMAT _d3d12Format);
       
											            Texture				                (const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot);
											            ~Texture			                ();

		const D3D12_CPU_DESCRIPTOR_HANDLE &             getd3d12RTVHandle	                () const;
        const D3D12_CPU_DESCRIPTOR_HANDLE &             getd3d12DSVHandle	                () const;

        void                                            setSubResourceData(core::uint _index, core::size_t _offset);
        const SubResourceData &                         getSubResourceData(core::uint _index) const;

	private:
        static const inline D3D12_CPU_DESCRIPTOR_HANDLE s_d3d12invalidSRVHandle = { (core::uint)-1 };
		static const inline D3D12_CPU_DESCRIPTOR_HANDLE s_d3d12invalidRTVHandle = { (core::uint)-1 };
        static const inline D3D12_CPU_DESCRIPTOR_HANDLE s_d3d12invalidDSVHandle = { (core::uint)-1 };
        
        struct D3D12_SUBRESOURCE_FOOTPRINT
        {
            D3D12_PLACED_SUBRESOURCE_FOOTPRINT  m_layout;
            core::uint                          m_numRows;
            core::size_t                        m_rowSizeInBytes;
            core::size_t                        m_totalSize;
        };

        D3D12_CPU_DESCRIPTOR_HANDLE			            m_d3d12RTVHandle = s_d3d12invalidRTVHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE			            m_d3d12DSVHandle = s_d3d12invalidDSVHandle;

        core::vector<SubResourceData>                   m_subResourceData;
	};
}