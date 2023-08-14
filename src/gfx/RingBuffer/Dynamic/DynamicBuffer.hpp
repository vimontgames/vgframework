#include "DynamicBuffer.h"

namespace vg::gfx
{
    core::vector<DynamicBuffer *> DynamicBuffer::s_DynamicBuffers;

    //--------------------------------------------------------------------------------------
    DynamicBuffer::DynamicBuffer(const core::string & _name, core::uint _size) :
        RingBuffer(_name, _size)
    {
        s_DynamicBuffers.push_back(this);
    }

    //--------------------------------------------------------------------------------------
    DynamicBuffer::~DynamicBuffer()
    {
        s_DynamicBuffers.remove(this);
    }

    ////--------------------------------------------------------------------------------------
    //core::u8 * DynamicBuffer::map(core::size_t _size, core::size_t _aligment)
    //{
    //    return RingBuffer::map(_size, _aligment);
    //}
    //
    ////--------------------------------------------------------------------------------------
    //void DynamicBuffer::unmap(CommandList * _cmdList, Buffer * _buffer, core::u8 * _dst)
    //{
    //    //if (nullptr != _dst)
    //    //{
    //    //    _cmdList->copyBuffer(pair.first, pair.second);
    //    //}
    //    RingBuffer::unmap();
    //}

    //--------------------------------------------------------------------------------------
    void DynamicBuffer::BeginFrame()
    {
        for (int i = 0; i < s_DynamicBuffers.size(); ++i)
        {
            DynamicBuffer * dynBuf = s_DynamicBuffers[i];
            dynBuf->flush();
        }
    }

    //--------------------------------------------------------------------------------------
    void DynamicBuffer::flush()
    {
        lock_guard<mutex> lock(m_uploadMutex);
        m_offsetStart = m_offsetCur;
    }
}