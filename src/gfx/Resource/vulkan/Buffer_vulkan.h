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

        static core::size_t getRequiredUploadSize(const BufferDesc & _bufferDesc);

    private:
        VkBufferView    m_vkBufferView = VK_NULL_HANDLE;
    };
}