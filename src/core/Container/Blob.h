#pragma once

namespace vg::core
{
    class Blob
    {
    public:
        Blob()  :
            m_data(nullptr),
            m_size(0)
        {

        }

        Blob(const void * _data, const uint _size) :
            m_data((void*)_data),
            m_size(_size)
        {

        }

        const void * data() const { return m_data; }
        uint size() const { return m_size; }

        static Blob Copy(const void * _data, const uint _size)
        {
            void * data = malloc(_size);
            memcpy(data, _data, _size);
            return Blob(data, _size);
        }

        void Release()
        {
            free(m_data);
            m_size = 0;
        }

    private:
        void * m_data;
        uint m_size;
    };
}