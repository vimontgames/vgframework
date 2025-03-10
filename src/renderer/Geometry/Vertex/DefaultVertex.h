#pragma once

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Struct used foreach vertex format
    // This is a format used by the GPU so if packing happens here then 
    // the corresponding unpack code should be implemented in vertex.hlsli for the vertex format
    //--------------------------------------------------------------------------------------
    struct DefaultVertex
    {
        // Every vertex format struct should implement a "set" func to initialize vertex contents from Fat vertex format
        void set(const FatVertex & _vtx)
        {
            setPos((core::float3)_vtx.pos);
            setColor((core::float4)_vtx.color);

            nrm[0] = _vtx.nrm.x;    nrm[1] = _vtx.nrm.y;    nrm[2] = _vtx.nrm.z;    // TODO: pack normal
            bin[0] = _vtx.bin.x;    bin[1] = _vtx.bin.y;    bin[2] = _vtx.bin.z;    // TODO: pack binormal
            tan[0] = _vtx.tan.x;    tan[1] = _vtx.tan.y;    tan[2] = _vtx.tan.z;    // TODO: pack tangent

            uv[0][0] = _vtx.uv[0].x;  uv[0][1] = _vtx.uv[0].y;                      // TODO: pack UV0
            uv[1][0] = _vtx.uv[1].x;  uv[1][1] = _vtx.uv[1].y;                      // TODO: pack UV1
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
        float       nrm[3];
        float       bin[3];
        float       tan[3];
        float       uv[2][2];
        core::u32   color;
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <> struct VertexStorage<VertexFormat::Default> { using type = DefaultVertex; };
    VG_STATIC_ASSERT(sizeof(DefaultVertex) == getVertexFormatStride(VertexFormat::Default), "DefaultVertex struct size does not match HLSL stride");
    VG_STATIC_ASSERT_IS_ALIGNED(sizeof(DefaultVertex), GPU_VERTEXBUFFER_OFFSET_ALIGNMENT, "DefaultVertex struct size does not match GPU_VERTEXBUFFER_OFFSET_ALIGNMENT");
}