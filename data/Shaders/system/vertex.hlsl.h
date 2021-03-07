#pragma once

namespace vg::graphics::driver
{
    // Imaginary vertex format with all possible attributes unpacked
    // We don't care about packing here as this format will only be used to fill the actual vertex buffers
    struct FatVertex
    {
        core::float4    pos;
        core::float2    uv[2];
        core::float4    nrm;
    };

    // List every vertex format here
    enum class VertexFormat : core::u8
    {
        Simple = 0,
    };

    // Struct used foreach vertex format
    // This is a format used by the GPU so if packing happens here then 
    // the corresponding unpack code should be implemented in vertex.hlsli for the vertex format
    struct SimpleVertex
    {
        // Every vertex format struct should implement a "set" func to initialize vertex contents from Fat vertex format
        void set(const FatVertex & _vtx)
        {
            pos[0] = _vtx.pos.x;    pos[1] = _vtx.pos.y;    pos[2] = _vtx.pos.z;    // TODO: pack position
            nrm[0] = _vtx.nrm.x;    nrm[1] = _vtx.nrm.y;    nrm[2] = _vtx.nrm.z;    // TODO: pack normal
             uv[0] = _vtx.uv[0].x;  uv[1] = _vtx.uv[0].y;                           // TODO: pack UV0
        }

        float pos[3];
        float nrm[3];
        float uv[2];
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <VertexFormat F> struct VertexStorage;
    template <> struct VertexStorage<VertexFormat::Simple> { using type = SimpleVertex;  };

    // template vertex that can support any vertex format
    template <VertexFormat F> struct Vertex
    {
        public:
            typename VertexStorage<F>::type data;
    };
}