#pragma once
#include "gfx/Resource/Buffer.h"

namespace vg::gfx::vulkan
{
    class Buffer : public base::Buffer
    {
        using super = base::Buffer;

    public:
        Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot);
        ~Buffer();

    private:
        VkBufferView    m_vkBufferView;
    };
}