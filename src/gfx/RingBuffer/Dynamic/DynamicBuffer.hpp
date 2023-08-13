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

    //--------------------------------------------------------------------------------------
    void DynamicBuffer::Flush()
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