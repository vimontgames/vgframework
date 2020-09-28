#pragma once

namespace vg::graphics::driver::dx12
{
    class BindlessTable : public base::BindlessTable
    {
        using super = base::BindlessTable;

    public:
        BindlessTable();
        ~BindlessTable();

        void beginFrame();

        D3D12_CPU_DESCRIPTOR_HANDLE getd3d12DescriptorHandle(BindlessHandle _handle) const;
        ID3D12DescriptorHeap * getd3d12GPUDescriptorHeap() const { return m_srvGPUDescriptorHeap; }

        void copyTextureHandle(core::uint _slot, driver::Texture * _texture);

    private:
        static const inline core::uint	s_invalidSrvDescriptorSize = (core::uint) - 1;
        ID3D12DescriptorHeap *          m_srvCPUDescriptorHeap = nullptr;
        ID3D12DescriptorHeap *          m_srvGPUDescriptorHeap = nullptr;
        core::uint                      m_srcDescriptorHeapSize = s_invalidSrvDescriptorSize;
    };
}