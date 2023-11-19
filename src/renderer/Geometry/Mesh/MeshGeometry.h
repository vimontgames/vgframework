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

        void                        setVertexFormat         (VertexFormat _vtxFmt);
        VertexFormat                getVertexFormat         () const;

        void                        setIndexBuffer          (gfx::Buffer * _ib, core::u32 _offset = 0);
        void                        setVertexBuffer         (gfx::Buffer * _vb, core::u32 _offset = 0);

        gfx::Buffer *               getIndexBuffer          () const;
        core::u32                   getIndexBufferOffset    () const;

        gfx::Buffer *               getVertexBuffer         () const;
        core::u32                   getVertexBufferOffset   () const;

        core::u32                   addBatch                (core::u32 _count, core::u32 _offset = 0);

        const core::vector<Batch> & batches                 () const;

    private:
        VertexFormat                m_vertexFormat = (VertexFormat)-1;
        gfx::Buffer *               m_indexBuffer = nullptr;
        gfx::Buffer *               m_vertexBuffer = nullptr;
        core::u32                   m_indexBufferOffset = 0;
        core::u32                   m_vertexBufferOffset = 0;
        core::vector<Batch>         m_batches;
    };
}