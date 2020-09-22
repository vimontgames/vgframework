#include VG_GFXAPI_IMPL(Buffer)

namespace vg::graphics::driver
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData) :
            m_bufDesc(_bufDesc)
        {
            VG_DEBUGPRINT("Init Buffer \"%s\" (", _name.c_str());
            VG_DEBUGPRINT("%u", _bufDesc.elementSize);
            if (_bufDesc.elementCount > 1)
                VG_DEBUGPRINT("x%u", _bufDesc.elementCount);
           
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
    Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData) :
        super::Buffer(_bufDesc, _name, _initData)
    {

    }

    //--------------------------------------------------------------------------------------
    Buffer::~Buffer()
    {

    }
}