#pragma push_macro("new")
#undef new
#include "D3D12MemoryAllocator/src/D3D12MemAlloc.cpp"
#pragma pop_macro("new")

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    MemoryAllocator::MemoryAllocator() :
        super::MemoryAllocator()
    {
        auto * device = gfx::Device::get();
        VG_ASSERT(device);

        D3D12MA::ALLOCATOR_DESC desc = {};
        desc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        desc.pDevice = device->getd3d12Device();
        desc.pAdapter = device->getd3d12Adapter();
        desc.PreferredBlockSize = 0; // default
        desc.pAllocationCallbacks = nullptr;

        VG_VERIFY_SUCCEEDED(D3D12MA::CreateAllocator(&desc, &m_d3d12memoryAllocator));
    }

    //--------------------------------------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {
        VG_SAFE_RELEASE(m_d3d12memoryAllocator);
    }
}