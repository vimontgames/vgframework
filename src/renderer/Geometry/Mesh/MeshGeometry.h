#pragma once

#include "renderer/Geometry/Geometry.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Geometry/Vertex/VertexFormat.h"

namespace vg::gfx
{
    class Buffer;
}

namespace vg::renderer
{
    class MeshGeometry : public Geometry
    {
    public:
        VG_CLASS_DECL(MeshGeometry, Geometry)

                                                MeshGeometry            (const core::string & _name, core::IObject * _parent);
                                                ~MeshGeometry           ();

        void                                    setVertexFormat         (VertexFormat _vtxFmt);
        VG_INLINE VertexFormat                  getVertexFormat         () const;

        VG_INLINE core::uint                    getIndexCount           () const;
        VG_INLINE core::uint                    getIndexSize            () const;
        VG_INLINE core::uint                    getVertexCount          () const;
        VG_INLINE core::uint                    getVertexSize           () const;

        void                                    setIndexBuffer          (gfx::Buffer * _ib, core::u32 _offset = 0);
        void                                    setVertexBuffer         (gfx::Buffer * _vb, core::u32 _offset = 0);

        VG_INLINE gfx::Buffer *                 getIndexBuffer          () const;
        VG_INLINE core::u32                     getIndexBufferOffset    () const;

        VG_INLINE gfx::Buffer *                 getVertexBuffer         () const;
        VG_INLINE core::u32                     getVertexBufferOffset   () const;

        core::u32                               addBatch                (core::u32 _count, core::u32 _offset = 0);

        VG_INLINE const core::vector<Batch> &   batches                 () const;

    private:
        VertexFormat                            m_vertexFormat = (VertexFormat)-1;
        core::uint                              m_indexCount = 0;
        core::u8                                m_indexSize = 0;
        core::uint                              m_vertexCount = 0;
        core::u8                                m_vertexSize = 0;
        gfx::Buffer *                           m_indexBuffer = nullptr;
        gfx::Buffer *                           m_vertexBuffer = nullptr;
        core::u32                               m_indexBufferOffset = 0;
        core::u32                               m_vertexBufferOffset = 0;
        core::vector<Batch>                     m_batches;
    };
}

#if VG_ENABLE_INLINE
#include "MeshGeometry.inl"
#endif