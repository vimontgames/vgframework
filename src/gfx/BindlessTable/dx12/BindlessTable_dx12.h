#pragma once

namespace vg::gfx
{
    class TLAS;
}

namespace vg::gfx::dx12
{
    class BindlessTable : public base::BindlessTable
    {
        using super = base::BindlessTable;

    public:
        BindlessTable();
        ~BindlessTable();

        void                            updateBindlessTLASHandle    (const BindlessTLASHandle & _handle, const gfx::TLAS * _tlas);

        D3D12_CPU_DESCRIPTOR_HANDLE     getd3d12CPUDescriptorHandle (BindlessHandle _handle) const;
        D3D12_GPU_DESCRIPTOR_HANDLE     getd3d12GPUDescriptorHandle (BindlessHandle _handle) const;

        ID3D12DescriptorHeap *          getd3d12GPUDescriptorHeap   () const { return m_srvGPUDescriptorHeap; }
        core::uint                      getd3d12DescriptorSize      () const { return m_srcDescriptorHeapSize; }

        void                            copyTextureHandle           (core::uint _slot, gfx::Texture * _texture);
        void                            updated3d12descriptor       (BindlessHandle _handle);

    private:
        static const inline core::uint	s_invalidSrvDescriptorSize = (core::uint) - 1;

        ID3D12DescriptorHeap *          m_srvCPUDescriptorHeap     = nullptr;
        ID3D12DescriptorHeap *          m_srvGPUDescriptorHeap     = nullptr;
        core::uint                      m_srcDescriptorHeapSize    = s_invalidSrvDescriptorSize;
    };
}