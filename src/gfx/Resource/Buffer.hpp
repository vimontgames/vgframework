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
            //VG_DEBUGPRINT("[Device] Init Buffer \"%s\" (", _name.c_str());
            //
            //if (_bufDesc.elementSize > 1024 * 1024)
            //    VG_DEBUGPRINT("%uMB", _bufDesc.elementSize / (1024*1024));
            //else if (_bufDesc.elementSize > 1024)
            //    VG_DEBUGPRINT("%uKB", _bufDesc.elementSize/1024);
            //else
            //    VG_DEBUGPRINT("%uB", _bufDesc.elementSize);
            //
            //if (_bufDesc.elementCount > 1)
            //{
            //    VG_DEBUGPRINT(" x %u", _bufDesc.elementCount);
            //
            //    if (_bufDesc.size() > 1024 * 1024)
            //        VG_DEBUGPRINT(" = %uMB", _bufDesc.size() / (1024 * 1024));
            //    else if (_bufDesc.size() > 1024)
            //        VG_DEBUGPRINT(" = %uKB", _bufDesc.size() / 1024);
            //    else
            //        VG_DEBUGPRINT(" = %uB", _bufDesc.size());
            //}
            //
            //bool first = true;
            //for (uint i = 0; i < countBits(_bufDesc.flags); ++i)
            //{
            //    const BufferFlags flag = BufferFlags(1 << i);
            //    if (asBool(_bufDesc.flags & flag))
            //    {
            //        VG_DEBUGPRINT("%c%s", first ? ' ' : '|', asString(flag).c_str());
            //        first = false;
            //    }
            //}
            //VG_DEBUGPRINT(")\n");
        }

        //--------------------------------------------------------------------------------------
        Buffer::~Buffer()
        {

        }
    }

    //--------------------------------------------------------------------------------------
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
        super::Buffer(_bufDesc, _name, _initData, _reservedSlot)
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