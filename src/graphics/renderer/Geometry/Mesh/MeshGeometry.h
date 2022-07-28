#pragma once

#include "graphics/renderer/Geometry/Geometry.h"
#include "graphics/renderer/Geometry/Batch.h"

namespace vg::graphics::driver
{
    class Buffer;
}

namespace vg::graphics::renderer
{
    class MeshGeometry : public Geometry
    {
        using super = Geometry;

    public:
        const char *                getClassName            () const final { return "MeshGeometry"; }

                                    MeshGeometry            (const core::string & _name, core::IObject * _parent);
                                    ~MeshGeometry           ();

        static bool                 registerClass           (core::IFactory & _factory);
        static bool                 registerProperties      (core::IClassDesc & _desc);

        void                        setIndexBuffer          (driver::Buffer * _ib, core::u32 _offset = 0);
        void                        setVertexBuffer         (driver::Buffer * _vb, core::u32 _offset = 0);

        driver::Buffer *            getIndexBuffer          () const;
        core::u32                   getIndexBufferOffset    () const;

        driver::Buffer *            getVertexBuffer         () const;
        core::u32                   getVertexBufferOffset   () const;

        core::u32                   addBatch                (core::u32 _count, core::u32 _offset = 0);

        const core::vector<Batch> & batches                 () const;

    private:
        driver::Buffer *            m_indexBuffer = nullptr;
        driver::Buffer *            m_vertexBuffer = nullptr;
        core::u32                   m_indexBufferOffset = 0;
        core::u32                   m_vertexBufferOffset = 0;
        core::vector<Batch>         m_batches;
    };
}