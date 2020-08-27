#include "D3D12MemoryAllocator/src/D3D12MemAlloc.cpp"

namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    MemoryAllocator::MemoryAllocator() :
        super::MemoryAllocator()
    {
        auto * device = driver::Device::get();
        VG_ASSERT(device);

        D3D12MA::ALLOCATOR_DESC desc = {};
        desc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        desc.pDevice = device->getd3d12Device();
        desc.pAdapter = device->getd3d12Adapter();
        desc.PreferredBlockSize = 0; // default
        desc.pAllocationCallbacks = nullptr;

        VG_ASSERT_SUCCEEDED(D3D12MA::CreateAllocator(&desc, &m_d3d12memoryAllocator));
    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {
        VG_SAFE_RELEASE(m_d3d12memoryAllocator);
    }
}