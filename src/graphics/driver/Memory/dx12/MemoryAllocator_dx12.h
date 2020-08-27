#pragma once

#include "graphics/driver/Memory/MemoryAllocator.h"
#include "D3D12MemoryAllocator/src/D3D12MemAlloc.h"

namespace vg::graphics::driver::dx12
{
    class MemoryAllocator : public base::MemoryAllocator
    {
        using super = base::MemoryAllocator;

    public:
        MemoryAllocator();
        ~MemoryAllocator();

        D3D12MA::Allocator * getd3d12MemoryAllocator() { return m_d3d12memoryAllocator; }

    private:
        D3D12MA::Allocator * m_d3d12memoryAllocator = nullptr;
    };
}