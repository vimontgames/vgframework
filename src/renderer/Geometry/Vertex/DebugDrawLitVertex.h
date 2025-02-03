#pragma once

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Vertex Format used for debug draw lit primitives
    //--------------------------------------------------------------------------------------
    struct DebugDrawLitVertex
    {
        DebugDrawLitVertex(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, core::u32 _color = 0xFFFFFFFF, float _nx = 0.0f, float _ny = 0.0f, float _nz = 1.0f)
        {
            pos[0] = _x;
            pos[1] = _y;
            pos[2] = _z;
            color = _color;
            nrm[0] = _nx;
            nrm[1] = _ny;
            nrm[2] = _nz;
        }

        void set(const FatVertex & _vtx)
        {
            setPos((core::float3)_vtx.pos);
            setColor((core::float4)_vtx.color);
            setNormal(core::float3(_vtx.nrm.x, _vtx.nrm.y, _vtx.nrm.z));
        }

        void setPos(const core::float3 & _pos)
        {
            // TODO: pack position
            pos[0] = _pos.x;
            pos[1] = _pos.y;
            pos[2] = _pos.z;
        }

        core::float3 getPos() const
        {
            return core::float3(pos[0], pos[1], pos[2]);
        }

        void setNormal(const core::float3 & _nrm)
        {
            nrm[0] = _nrm.x;
            nrm[1] = _nrm.y;
            nrm[2] = _nrm.z;
        }

        void setColor(const core::float4 & _color)
        {
            color = packRGBA8(_color);
        }

        void setColor(core::uint _color)
        {
            color = _color;
        }

        float       pos[3];
        float       nrm[3];
        core::u32   color;
    };

    template <> struct VertexStorage<VertexFormat::DebugDrawLit> { using type = DebugDrawLitVertex; };
    VG_STATIC_ASSERT(sizeof(DebugDrawLitVertex) == getVertexFormatStride(VertexFormat::DebugDrawLit), "DebugDrawGeometryVertex struct size does not match HLSL stride");
    VG_STATIC_ASSERT_IS_ALIGNED(sizeof(DebugDrawLitVertex), GPU_VERTEXBUFFER_OFFSET_ALIGNMENT, "DebugDrawGeometryVertex struct size does not match GPU_VERTEXBUFFER_OFFSET_ALIGNMENT");
}