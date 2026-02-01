#pragma once

#include "Shaders/system/vertex.hlsli"
#include "Shaders/system/packing.hlsli"

namespace vg::renderer
{
    // Imaginary vertex format with all possible attributes unpacked (this format will only be used to fill the actual vertex buffers)
    struct FatVertex
    {
        core::StorageFloat3 pos;
        core::StorageFloat4 nrm;
        core::StorageFloat4 bin;
        core::StorageFloat4 tan;
        core::StorageFloat4 color;
        core::StorageFloat2 uv[2];
        u16                 indices[4];
        u8                  weights[4];

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
            return (pos == _other.pos) && (nrm == _other.nrm) && (bin == _other.bin) && (tan == _other.tan) && (color == _other.color) && (uv[0] == _other.uv[0]) && (uv[1] == _other.uv[1]);
        }
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <VertexFormat F> struct VertexStorage;
}

// Include all vertex format headers here
#include "DefaultVertex.h"
#include "SkinningVertex.h"
#include "DebugDrawUnlitVertex.h"
#include "DebugDrawLitVertex.h"
#include "ParticleQuadVertex.h"
