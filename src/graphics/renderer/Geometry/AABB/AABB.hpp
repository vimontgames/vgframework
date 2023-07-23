#include "AABB.h"

#if !VG_ENABLE_INLINE
#include "AABB.inl"
#endif

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    bool AABB::read(io::Buffer & _buffer)
    {
        _buffer.read(&m_min);
        _buffer.read(&m_max);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AABB::write(io::Buffer & _buffer) const
    {
        _buffer.write(m_min);
        _buffer.write(m_max);

        return true;
    }
}