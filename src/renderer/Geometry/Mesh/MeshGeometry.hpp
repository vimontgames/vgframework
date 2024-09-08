#include "MeshGeometry.h"

using namespace vg::core;

#if !VG_ENABLE_INLINE
#include "MeshGeometry.inl"
#endif

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(MeshGeometry, "Mesh Geometry");

    //--------------------------------------------------------------------------------------
    bool MeshGeometry::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(MeshGeometry, VertexFormat, m_vertexFormat, "Vertex Format", PropertyFlags::ReadOnly);
        setPropertyDescription(MeshGeometry, m_vertexFormat, "The vertex format used by the geometry determines the available attributes");

        registerPropertyEx(MeshGeometry, m_indexCount, "Index Count", PropertyFlags::ReadOnly);
        setPropertyDescription(MeshGeometry, m_indexCount, "Total index count for geometry");

        registerPropertyEx(MeshGeometry, m_indexSize, "Index Size", PropertyFlags::ReadOnly);
        setPropertyDescription(MeshGeometry, m_indexSize, "Index size used by the geometry");

        registerPropertyEx(MeshGeometry, m_vertexCount, "Vertex Count", PropertyFlags::ReadOnly);
        setPropertyDescription(MeshGeometry, m_vertexCount, "Total vertex count for geometry");

        registerPropertyEx(MeshGeometry, m_vertexSize, "Vertex Size", PropertyFlags::ReadOnly);
        setPropertyDescription(MeshGeometry, m_vertexSize, "Per-vertex size of the vertex format used");

        registerPropertyEx(MeshGeometry, m_indexBufferOffset, "IB Offset", PropertyFlags::ReadOnly | PropertyFlags::Debug);
        setPropertyDescription(MeshGeometry, m_indexBufferOffset, "Index buffer offset");

        registerPropertyEx(MeshGeometry, m_vertexBufferOffset, "VB Offset", PropertyFlags::ReadOnly | PropertyFlags::Debug);
        setPropertyDescription(MeshGeometry, m_vertexBufferOffset, "Vertex buffer offset");

        registerPropertyObjectVectorEx(MeshGeometry, m_batches, Batch, "Batches", PropertyFlags::ReadOnly);

        return true;
    }

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
    void MeshGeometry::setVertexFormat(VertexFormat _vtxFmt)
    {
        m_vertexFormat = _vtxFmt;
    }

    //--------------------------------------------------------------------------------------
    void MeshGeometry::setIndexBuffer(gfx::Buffer * _ib, core::u32 _offset)
    {
        if (_ib != m_indexBuffer)
        {
            VG_SAFE_INCREASE_REFCOUNT(_ib);
            VG_SAFE_RELEASE(m_indexBuffer);
            m_indexBuffer = _ib;

            if (nullptr != _ib)
            {
                const auto & desc = _ib->getBufDesc();
                m_indexCount = desc.getElementCount();
                m_indexSize = desc.getElementSize();
            }
            else
            {
                m_indexCount = 0;
                m_indexSize = 0;
            }
        }

        if (_offset != m_indexBufferOffset)
            m_indexBufferOffset = _offset;
    }

    //--------------------------------------------------------------------------------------
    void MeshGeometry::setVertexBuffer(gfx::Buffer * _vb, core::u32 _offset)
    {
        if (_vb != m_vertexBuffer)
        {
            VG_SAFE_INCREASE_REFCOUNT(_vb);
            VG_SAFE_RELEASE(m_vertexBuffer);
            m_vertexBuffer = _vb;

            if (nullptr != _vb)
            {
                const auto & desc = _vb->getBufDesc();
                m_vertexCount = desc.getElementCount();
                m_vertexSize = desc.getElementSize();
            }
            else
            {
                m_vertexCount = 0;
                m_vertexSize = 0;
            }
        }

        if (_offset != m_vertexBufferOffset)
            m_vertexBufferOffset = _offset;
    }

    //--------------------------------------------------------------------------------------
    core::u32 MeshGeometry::addBatch(const core::string & _name, core::u32 _count, core::u32 _offset)
    {
        const core::u32 count = (core::u32)m_batches.size();

        Batch & batch = m_batches.emplace_back();
        batch.setName(_name);
        batch.count = _count;
        batch.offset = _offset;

        return count;
    }
}