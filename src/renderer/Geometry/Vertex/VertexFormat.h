#pragma once

#include "Shaders/system/vertex.hlsli"
#include "shaders/system/packing.hlsli"

namespace vg::renderer
{
    // Imaginary vertex format with all possible attributes unpacked
    // We don't care about packing here as this format will only be used to fill the actual vertex buffers
    struct FatVertex
    {
        core::float4    pos;
        core::float4    nrm;
        core::float4    bin;
        core::float4    tan;
        core::float4    color;
        core::float2    uv[2];
        uint            indices[4];
        float           weights[4];

        struct hash
        {
            inline core::size_t operator() (const FatVertex & _this) const
            {
                core::size_t hash = core::Hash(_this.pos) ^ core::Hash(_this.nrm) ^ core::Hash(_this.nrm) ^ core::Hash(_this.bin) ^ core::Hash(_this.tan) ^ core::Hash(_this.color) ^ core::Hash(_this.uv[0]) ^ core::Hash(_this.uv[1]);
                return hash;
            }
        };

        inline bool operator == (const FatVertex & _other) const
        {
            return all(pos == _other.pos) && all(nrm == _other.nrm) && all(bin == _other.bin) && all(tan == _other.tan) && all(color == _other.color) && all(uv[0] == _other.uv[0]) && all(uv[1] == _other.uv[1]);
        }
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <VertexFormat F> struct VertexStorage;
}

// Include all vertex format headers here
#include "DefaultVertex.h"
#include "SkinningVertex.h"
#include "DebugDrawVertex.h"
