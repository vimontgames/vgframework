#ifdef VG_DX12
#include "dx12/Buffer_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/Buffer_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

#if !VG_ENABLE_INLINE
#include "Buffer.inl"
#endif

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData) :
            super(_name, nullptr),
            m_bufDesc(_bufDesc)
        {
        }

        //--------------------------------------------------------------------------------------
        Buffer::~Buffer()
        {
        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot, CommandList * _cmdList) :
        super::Buffer(_bufDesc, _name, _initData, _reservedSlot, _cmdList)
    {

    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {
        auto * device = Device::get();
        auto * bindlessTable = device->getBindlessTable();

        if (m_bufferHandle.isValid())
            bindlessTable->freeBindlessBufferHandle(m_bufferHandle);

        if (m_rwBufferHandle.isValid())
            bindlessTable->freeBindlessRWBufferHandle(m_rwBufferHandle);
    }
}