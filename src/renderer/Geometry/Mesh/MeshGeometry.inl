namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    VG_INLINE VertexFormat MeshGeometry::getVertexFormat() const
    {
        VG_ASSERT(m_vertexFormat != (VertexFormat)-1);
        return m_vertexFormat;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint MeshGeometry::getIndexCount() const
    {
        return m_indexCount;
    }
    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint MeshGeometry::getIndexSize() const
    {
        return m_indexSize;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint MeshGeometry::getVertexCount() const
    {
        return m_vertexCount;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint MeshGeometry::getVertexSize() const
    {
        return m_vertexSize;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Buffer * MeshGeometry::getIndexBuffer() const
    {
        return m_indexBuffer;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::u32 MeshGeometry::getIndexBufferOffset() const
    {
        return m_indexBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Buffer * MeshGeometry::getVertexBuffer() const
    {
        return m_vertexBuffer;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::u32 MeshGeometry::getVertexBufferOffset() const
    {
        return m_vertexBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<Batch> & MeshGeometry::batches() const
    {
        return m_batches;
    }
}