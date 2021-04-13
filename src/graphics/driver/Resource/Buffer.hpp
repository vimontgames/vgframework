#include VG_GFXAPI_IMPL(Buffer)

namespace vg::graphics::driver
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, const void * _initData) :
            m_bufDesc(_bufDesc)
        {
            VG_DEBUGPRINT("Init Buffer \"%s\" (", _name.c_str());

            if (_bufDesc.elementSize > 1024 * 1024)
                VG_DEBUGPRINT("%uMB", _bufDesc.elementSize / (1024*1024));
            else if (_bufDesc.elementSize > 1024)
                VG_DEBUGPRINT("%uKB", _bufDesc.elementSize/1024);
            else
                VG_DEBUGPRINT("%uB", _bufDesc.elementSize);

            if (_bufDesc.elementCount > 1)
            {
                VG_DEBUGPRINT(" x %u", _bufDesc.elementCount);

                if (_bufDesc.size() > 1024 * 1024)
                    VG_DEBUGPRINT(" = %uMB", _bufDesc.size() / (1024 * 1024));
                else if (_bufDesc.size() > 1024)
                    VG_DEBUGPRINT(" = %uKB", _bufDesc.size() / 1024);
                else
                    VG_DEBUGPRINT(" = %uB", _bufDesc.size());
            }
           
            bool first = true;
            for (uint i = 0; i < countBits(_bufDesc.flags); ++i)
            {
                const BufferFlags flag = BufferFlags(1 << i);
                if (asBool(_bufDesc.flags & flag))
                {
                    VG_DEBUGPRINT("%c%s", first ? ' ' : '|', asString(flag).c_str());
                    first = false;
                }
            }
            int i = 0;
            VG_DEBUGPRINT(")\n");
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
        device->getBindlessTable()->freeBindlessBufferHandle(m_bindlessSRVHandle);
    }
}