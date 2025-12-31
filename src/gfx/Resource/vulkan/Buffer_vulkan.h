#pragma once
#include "gfx/Resource/Buffer.h"

namespace vg::gfx::vulkan
{
    class Buffer : public base::Buffer
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(Buffer, base::Buffer);

        Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot);
        ~Buffer();

        static bool getRequiredUploadSizeAndAlignment(const BufferDesc & _bufferDesc, core::size_t & _size, core::size_t & _alignment);

    private:
        VkBufferView    m_vkBufferView = VK_NULL_HANDLE;
    };
}