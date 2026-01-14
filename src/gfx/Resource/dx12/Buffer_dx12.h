#pragma once
#include "gfx/Resource/Buffer.h"

namespace vg::gfx::dx12
{
    class Buffer : public base::Buffer
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(Buffer, base::Buffer);        

        Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot, gfx::CommandList * _cmdList = nullptr);
        ~Buffer(); 

        static D3D12_RESOURCE_DESC      getd3d12ResourceDesc(const BufferDesc & _bufDesc);
        static D3D12MA::ALLOCATION_DESC getd3d12AllocationDesc(const BufferDesc & _bufDesc);
        static bool getRequiredUploadSizeAndAlignment(const BufferDesc & _bufferDesc, core::size_t & _size, core::size_t & _alignment);
    };
}