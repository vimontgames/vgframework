#pragma once

#include "graphics/driver/device/Device.h"

namespace D3D12MA
{
    class Allocator;
}

namespace vg::graphics::driver::dx12
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

            //D3D12_CPU_DESCRIPTOR_HANDLE   allocSRVHandle              (core::uint _count = 1);
            //void							freeSRVHandle               (D3D12_CPU_DESCRIPTOR_HANDLE & _hSRV);

		protected:
			void							init						(const DeviceParams & _params);
			void							deinit						();

            void                            waitGPUIdle                 ();

			void							beginFrame					();
			void							endFrame					();
			
		private:
			IDXGISwapChain3 *				created3d12SwapChain		(HWND _winHandle, core::uint _width, core::uint _height);

        private:
            D3D12Device *					m_d3d12device				= nullptr;
			ID3D12Debug *					m_d3d12debug				= nullptr;
			D3D_FEATURE_LEVEL				m_level						= (D3D_FEATURE_LEVEL)0;	// TODO: caps struct
			
			IDXGISwapChain3 *               m_dxgiSwapChain				= nullptr;
			IDXGIAdapter1 *					m_dxgiAdapter				= nullptr;
			IDXGIFactory5 *					m_dxgiFactory				= nullptr;
			
			static const inline core::uint	s_invalidRenderTargetDescriptorSize = (core::uint) - 1;
			ID3D12DescriptorHeap *			m_renderTargetDescriptorHeap = nullptr;
			core::uint						m_renderTargetDescriptorAllocated = max_backbuffer_count;
			core::uint						m_renderTargetDescriptorUsed = 0;
			core::uint						m_renderTargetDescriptorSize = s_invalidRenderTargetDescriptorSize;

			// backbuffer
			//HANDLE							m_frameFenceEvents;
			//ID3D12Fence *					m_frameFences[max_frame_latency];
			//UINT64							m_currentFenceValue;
			//UINT64							m_fenceValues[max_frame_latency];

            UINT64                          mNextFrameFence;
            ID3D12Fence *                   mFence;
            HANDLE                          mFenceEvent;
            HANDLE                          mSwapEvent;
	};
}