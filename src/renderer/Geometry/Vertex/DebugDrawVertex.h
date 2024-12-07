#pragma once

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Vertex Format used for debug draw lines, boxes etc...
    //--------------------------------------------------------------------------------------
    struct DebugDrawVertex
    {
        DebugDrawVertex(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, core::u32 _color = 0xFFFFFFFF)
        {
            pos[0] = _x;
            pos[1] = _y;
            pos[2] = _z;
            color = _color;
        }

        // Every vertex format struct should implement a "set" func to initialize vertex contents from Fat vertex format
        void set(const FatVertex & _vtx)
        {
            setPos((core::float3)_vtx.pos);
            setColor((core::float4)_vtx.color);
        }

        void setPos(const core::float3 & _pos)
        {
            // TODO: pack position
            pos[0] = _pos.x;
            pos[1] = _pos.y;
            pos[2] = _pos.z;
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
        core::u32   color;
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <> struct VertexStorage<VertexFormat::DebugDraw> { using type = DebugDrawVertex; };
    VG_STATIC_ASSERT(sizeof(DebugDrawVertex) == getVertexFormatStride(VertexFormat::DebugDraw), "DebugDrawVertex struct size does not match HLSL stride");
    VG_STATIC_ASSERT_IS_ALIGNED(sizeof(DebugDrawVertex), GPU_VERTEXBUFFER_OFFSET_ALIGNMENT, "DebugDrawVertex struct size does not match GPU_VERTEXBUFFER_OFFSET_ALIGNMENT");
}