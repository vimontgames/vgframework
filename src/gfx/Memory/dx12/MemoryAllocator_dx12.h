#pragma once

#include "gfx/Memory/MemoryAllocator.h"

#pragma push_macro("new")
#undef new
#include "D3D12MemoryAllocator/src/D3D12MemAlloc.h"
#pragma pop_macro("new")

namespace vg::gfx::dx12
{
    class MemoryAllocator : public base::MemoryAllocator
    {
        using super = base::MemoryAllocator;

    public:
        MemoryAllocator();
        ~MemoryAllocator();

        D3D12MA::Allocator * getd3d12MemoryAllocator() const { return m_d3d12memoryAllocator; }

    private:
        D3D12MA::Allocator * m_d3d12memoryAllocator = nullptr;
    };
}