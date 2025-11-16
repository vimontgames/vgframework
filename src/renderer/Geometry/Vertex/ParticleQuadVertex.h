#pragma once
namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // This struct represents a Particle 'vertex' that will be used to unpack four vertices
    //--------------------------------------------------------------------------------------
    struct ParticleQuadVertex
    {
        void setPos(const core::float3 & _pos)
        {
            pos[0] = _pos.x;
            pos[1] = _pos.y;
            pos[2] = _pos.z;
        }

        void setFrame(float _frame)
        {
            frame = _frame;
        }

        void setSize(const core::float2 & _size)
        {
            size[0] = _size.x;
            size[1] = _size.y;
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
        float       frame;
        float       size[2];
        core::u32   color;
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <> struct VertexStorage<VertexFormat::ParticleQuad> { using type = ParticleQuadVertex; };
    VG_STATIC_ASSERT(sizeof(ParticleQuadVertex) == getVertexFormatStride(VertexFormat::ParticleQuad), "ParticleQuadVertex struct size does not match HLSL stride");
    VG_STATIC_ASSERT_IS_ALIGNED(sizeof(ParticleQuadVertex), GPU_VERTEXBUFFER_OFFSET_ALIGNMENT, "ParticleQuadVertex struct size does not match GPU_VERTEXBUFFER_OFFSET_ALIGNMENT");
}