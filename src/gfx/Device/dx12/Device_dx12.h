#pragma once

#include "gfx/device/Device.h"
#include "core/Pool/IndexPool.h"

namespace D3D12MA
{
    class Allocator;
}

namespace vg::gfx::dx12
{
	class Device : public base::Device
	{
		using super = base::Device;

		public:
            D3D12Device *					getd3d12Device				() const;
            IDXGIAdapter1 *                 getd3d12Adapter             () const; 
            D3D12MA::Allocator *            getd3d12MemoryAllocator     () const;

			D3D12_CPU_DESCRIPTOR_HANDLE		allocRTVHandle	            (core::uint _count = 1);
			void							freeRTVHandle	            (D3D12_CPU_DESCRIPTOR_HANDLE & _hRTV);

            D3D12_CPU_DESCRIPTOR_HANDLE		allocDSVHandle              (core::uint _count = 1);
            void							freeDSVHandle               (D3D12_CPU_DESCRIPTOR_HANDLE & _hRTV);		

            void                            waitGPUIdle					();

			void							applyVSync					(VSync mode);
            void                            applyHDR                    (HDR _mode);
			void							applyColorSpace				(ColorSpace _mode);

			D3D12DXGISwapChain *            getd3d12SwapChain           () const { return m_dxgiSwapChain; }

		protected:
			void							init						(const DeviceParams & _params);
			void							deinit						();

            void                            resize                      (core::uint _width, core::uint _height);

			void							beginFrame					();
			void							endFrame					();
			
		private:
			D3D12DXGISwapChain *			created3d12SwapChain		(HWND _winHandle, core::uint _width, core::uint _height);
            void                            created3d12Backbuffers      ();
            void                            destroyd3d12Backbuffers     ();

			bool							isDeveloperModeEnabled		() const;
			bool							setStablePowerState			();

			void							checkHDRSupport				();

        private:
            D3D12Device *					m_d3d12device				= nullptr;
			ID3D12Debug *					m_d3d12debug				= nullptr;
			
			D3D12DXGISwapChain *            m_dxgiSwapChain				= nullptr;
            DXGI_SWAP_CHAIN_DESC1           m_dxgiSwapChainDesc;
			IDXGIAdapter1 *					m_dxgiAdapter				= nullptr;
			D3D12DXGIFactory *				m_dxgiFactory				= nullptr;
			
			ID3D12DescriptorHeap *			m_RTVDescriptorHeap = nullptr;
            core::IndexPool<core::u16,64>	m_RTVHandleIndexPool;
			core::uint						m_RTVDescriptorSize = 0;

            ID3D12DescriptorHeap *			m_DSVDescriptorHeap = nullptr;
            core::IndexPool<core::u16,16>	m_DSVHandleIndexPool;
            core::uint						m_DSVDescriptorSize = 0;

            UINT64                          m_nextFrameFence;
            ID3D12Fence *                   m_d3d12fence;
            HANDLE                          m_d3d12fenceEvent;
            HANDLE                          m_d3d12swapEvent;
	};
}