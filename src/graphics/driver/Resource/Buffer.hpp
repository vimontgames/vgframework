#include VG_GFXAPI_IMPL(Buffer)

namespace vg::graphics::driver
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        Buffer::Buffer(const BufferDesc & _bufDesc, const core::string & _name, void * _initData) :
            m_bufDesc(_bufDesc)
        {

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