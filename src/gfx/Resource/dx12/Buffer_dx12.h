#pragma once
#include "gfx/Resource/Buffer.h"

namespace vg::gfx::dx12
{
    class Buffer : public base::Buffer
    {
        using super = base::Buffer;

    public:
        static D3D12_RESOURCE_DESC      getd3d12ResourceDesc(const BufferDesc & _bufDesc);
        static D3D12MA::ALLOCATION_DESC getd3d12AllocationDesc(const BufferDesc & _bufDesc);

        Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot);
        ~Buffer();

    private:
        
    };
}