#pragma once

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Vertex Format used for debug draw lines, boxes etc...
    //--------------------------------------------------------------------------------------
    struct DebugDrawVertex
    {
        // Every vertex format struct should implement a "set" func to initialize vertex contents from Fat vertex format
        void set(const FatVertex & _vtx)
        {
            setPos(_vtx.pos.xyz);
            setColor(_vtx.color);
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
    VG_STATIC_ASSERT(sizeof(SkinningVertex_4Bones) == getVertexFormatStride(VertexFormat::Skinning_4Bones), "DebugDrawVertex struct size does not match HLSL stride");
}