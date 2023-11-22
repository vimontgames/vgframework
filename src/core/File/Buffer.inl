namespace vg::core::io
{
    //--------------------------------------------------------------------------------------
    inline Buffer::Buffer(const u8 * _data, size_t _size)
    {
        if (_size > 0)
        {
            VG_ASSERT(nullptr != _data);
            m_data.resize(_size);
            memcpy(&m_data[0], _data, _size);
        }
    }

    //--------------------------------------------------------------------------------------
    inline const u8 * Buffer::data() const
    { 
        return m_data.data(); 
    }

    //--------------------------------------------------------------------------------------
    inline size_t Buffer::size() const
    { 
        return m_data.size();
    }

    //--------------------------------------------------------------------------------------
    inline void Buffer::resize(core::size_t _size)
    {
        m_data.resize(_size);
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(u8 * _value)
    {
        return read(_value, sizeof(u8));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(u8 _value)
    {
        return write(&_value, sizeof(u8));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(u32 * _value)
    {
        return read(_value, sizeof(u32));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(u32 _value)
    {
        return write(&_value, sizeof(u32));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(float * _value)
    {
        return read(_value, sizeof(float));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const float & _value)
    {
        return write(&_value, sizeof(float));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(float2 * _value)
    {
        return read(_value, sizeof(float2));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const float2 & _value)
    {
        return write(&_value, sizeof(float2));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(float3 * _value)
    {
        return read(_value, sizeof(float3));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const float3 & _value)
    {
        return write(&_value, sizeof(float3));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(float4 * _value)
    {
        return read(_value, sizeof(float4));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const float4 & _value)
    {
        return write(&_value, sizeof(float4));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(quaternion * _value)
    {
        return read(_value, sizeof(quaternion));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const quaternion & _value)
    {
        return write(&_value, sizeof(quaternion));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(string * _string)
    {
        u32 len;
        read(&len);
        *_string = string((char*)&m_data[m_read], len);
        m_read += len + 1;

        return true;
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const string & _string)
    {
        const u32 len = (u32)_string.length();
        write(len);
        write(_string.c_str(), len + 1);

        return true;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> inline bool Buffer::read(vector<T> * _vector)
    {
        u32 count;
        if (read(&count))
        {
            if (count > 0)
            {
                _vector->resize(count);
                read(_vector->data(), count * sizeof(T));
            }
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> inline bool Buffer::write(const vector<T> & _vector)
    {
        const u32 count = (u32)_vector.size();
        write(count);
        if (count > 0)
            write(_vector.data(), count * sizeof(T));
        return true;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool Buffer::read(T * _value)
    {
        return read(_value, sizeof(T));
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool Buffer::write(const T & _value)
    {
        return write(&_value, sizeof(T));
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::read(void * _data, core::size_t _size)
    {
        if (m_read < m_data.size())
        {
            memcpy(_data, &m_data[m_read], _size);
            m_read += _size;

            return true;
        }
        else
        {
            VG_ASSERT(m_read < m_data.size());
            return false;
        }
    }

    //--------------------------------------------------------------------------------------
    inline bool Buffer::write(const void * _data, core::size_t _size)
    {
        const auto requested = m_write + _size;
        const auto current = m_data.size();

        if (requested > current)
            m_data.resize(requested);

        memcpy(&m_data[m_write], _data, _size);

        m_write += _size;

        return true;
    }
}