#pragma once
#include "graphics/driver/Resource/Buffer.h"

namespace vg::graphics::driver::vulkan
{
    class Buffer : public base::Buffer
    {
        using super = base::Buffer;

    public:
        Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData, ReservedSlot _reservedSlot);
        ~Buffer();

    private:
        VkBufferView    m_vkBufferView;
    };
}