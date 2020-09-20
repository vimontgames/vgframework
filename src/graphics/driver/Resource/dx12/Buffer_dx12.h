#pragma once

#include "graphics/driver/Resource/Buffer.h"

namespace D3D12MA
{
    class Allocation;
}

namespace vg::graphics::driver::dx12
{
    class Buffer : public base::Buffer
    {
        using super = base::Buffer;

    public:
        Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData);
        ~Buffer();

    private:
        
    };
}