#pragma once

#include "core/math/math.h"
#include "packing.hlsli"

namespace vg::core
{
    inline core::size_t Hash(const core::float2 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.y) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }

    inline core::size_t Hash(const core::float4 & _this)
    {
        core::size_t hash = core::hash<float>()(_this.x) ^ core::hash<float>()(_this.y) ^ 0xff51afd7ed558ccd ^ core::hash<float>()(_this.z) ^ 0xc4ceb9fe1a85ec53L ^ core::hash<float>()(_this.w) ^ 0xed558cc4ceb9fe1L;
        return hash;
    }
}

namespace vg::gfx
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

        struct hash
        {
            inline core::size_t operator() (const FatVertex & _this) const
            {
                core::size_t hash = core::Hash(_this.pos) ^ core::Hash(_this.nrm) ^ core::Hash(_this.nrm) ^ core::Hash(_this.bin) ^core::Hash(_this.tan) ^ core::Hash(_this.color) ^ core::Hash(_this.uv[0]) ^ core::Hash(_this.uv[1]);
                return hash;
            }
        };

        inline bool operator == (const FatVertex & _other) const
        {
            return all(pos == _other.pos) && all(nrm == _other.nrm) && all(bin == _other.bin) && all(tan == _other.tan) && all(color == _other.color) && all(uv[0] == _other.uv[0]) && all(uv[1] == _other.uv[1]);
        }
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
            setPos(_vtx.pos.xyz);
            setColor(_vtx.color);

            nrm[0] = _vtx.nrm.x;    nrm[1] = _vtx.nrm.y;    nrm[2] = _vtx.nrm.z;    // TODO: pack normal
            bin[0] = _vtx.bin.x;    bin[1] = _vtx.bin.y;    bin[2] = _vtx.bin.z;    // TODO: pack binormal
            tan[0] = _vtx.tan.x;    tan[1] = _vtx.tan.y;    tan[2] = _vtx.tan.z;    // TODO: pack tangent

            uv[0][0] = _vtx.uv[0].x;  uv[0][1] = _vtx.uv[0].y;                     // TODO: pack UV0
            uv[1][0] = _vtx.uv[1].x;  uv[1][1] = _vtx.uv[1].y;                     // TODO: pack UV1
        }

        void setPos(const float3 & _pos)
        {
            // TODO: pack position
            pos[0] = _pos.x;
            pos[1] = _pos.y;   
            pos[2] = _pos.z;    
        }

        void setColor(const float4 & _color)
        {
            color = packRGBA8(_color);
        }

        void setColor(uint _color)
        {
            color = _color;
        }

        float       pos[3];
        float       nrm[3];
        float       bin[3];
        float       tan[3];
        float       uv[2][2];
        core::u32   color;
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <VertexFormat F> struct VertexStorage;
    template <> struct VertexStorage<VertexFormat::Simple> { using type = SimpleVertex;  };
}