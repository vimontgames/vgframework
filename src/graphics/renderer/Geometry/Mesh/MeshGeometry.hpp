#include "MeshGeometry.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    MeshGeometry::MeshGeometry(const core::string & _name) : 
        Geometry(_name)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshGeometry::~MeshGeometry()
    {
        VG_SAFE_RELEASE(m_indexBuffer);
        VG_SAFE_RELEASE(m_vertexBuffer);
        m_batchs.clear();
    }

    //--------------------------------------------------------------------------------------
    void MeshGeometry::setIndexBuffer(driver::Buffer * _ib, core::u32 _offset)
    {
        if (_ib != m_indexBuffer)
        {
            VG_SAFE_INCREASE_REFCOUNT(_ib);
            VG_SAFE_RELEASE(m_indexBuffer);
            m_indexBuffer = _ib;
        }

        if (_offset != m_indexBufferOffset)
            m_indexBufferOffset = _offset;
    }

    //--------------------------------------------------------------------------------------
    void MeshGeometry::setVertexBuffer(driver::Buffer * _vb, core::u32 _offset)
    {
        if (_vb != m_vertexBuffer)
        {
            VG_SAFE_INCREASE_REFCOUNT(_vb);
            VG_SAFE_RELEASE(m_vertexBuffer);
            m_vertexBuffer = _vb;
        }

        if (_offset != m_vertexBufferOffset)
            m_vertexBufferOffset = _offset;
    }

    //--------------------------------------------------------------------------------------
    driver::Buffer * MeshGeometry::getIndexBuffer() const
    {
        return m_indexBuffer;
    }

    //--------------------------------------------------------------------------------------
    core::u32 MeshGeometry::getIndexBufferOffset() const
    {
        return m_indexBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    driver::Buffer * MeshGeometry::getVertexBuffer() const
    {
        return m_vertexBuffer;
    }

    //--------------------------------------------------------------------------------------
    core::u32 MeshGeometry::getVertexBufferOffset() const
    {
        return m_vertexBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    core::u32 MeshGeometry::addBatch(core::u32 _count, core::u32 _offset)
    {
        const core::u32 count = (core::u32)m_batchs.size();
        m_batchs.push_back({ _count, _offset });
        return count;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<Batch> & MeshGeometry::batches() const
    {
        return m_batchs;
    }
}