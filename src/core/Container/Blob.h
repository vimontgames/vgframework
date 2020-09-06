#pragma once

namespace vg::core
{
    class Blob
    {
    public:
        Blob(const void * _data, const uint _size) :
            m_data(_data),
            m_size(_size)
        {

        }

        const void * data() const { return m_data; }
        uint size() const { return m_size; }

    private:
        const void * m_data = nullptr;
        const uint m_size = 0;
    };
}