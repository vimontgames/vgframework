#include "MeshGeometry.h"

using namespace vg::core;

namespace vg::graphics::renderer
{
    VG_AUTO_REGISTER_CLASS(MeshGeometry);

    //--------------------------------------------------------------------------------------
    MeshGeometry::MeshGeometry(const core::string & _name, core::IObject * _parent) :
        Geometry(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshGeometry::~MeshGeometry()
    {
        VG_SAFE_RELEASE(m_indexBuffer);
        VG_SAFE_RELEASE(m_vertexBuffer);
        m_batches.clear();
    }

    //--------------------------------------------------------------------------------------
    bool MeshGeometry::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MeshGeometry, "Mesh Geometry", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MeshGeometry::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectVectorHelper(MeshGeometry, m_batches, Batch, "Batches", IProperty::Flags::None);

        _desc.registerPropertyHelper(MeshGeometry, m_indexBufferOffset, "IB Offset", IProperty::Flags::ReadOnly);
        _desc.registerPropertyHelper(MeshGeometry, m_vertexBufferOffset, "VB Offset", IProperty::Flags::ReadOnly);

        return true;
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
        const core::u32 count = (core::u32)m_batches.size();

        Batch batch;
              batch.count = _count;
              batch.offset = _offset;

        m_batches.push_back(batch);
        return count;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<Batch> & MeshGeometry::batches() const
    {
        return m_batches;
    }
}