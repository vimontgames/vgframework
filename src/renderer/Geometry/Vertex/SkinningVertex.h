#pragma once

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Skin vertex format
    //--------------------------------------------------------------------------------------
    struct SkinningVertex_4Bones
    {
        // Every vertex format struct should implement a "set" func to initialize vertex contents from Fat vertex format
        void set(const FatVertex & _vtx)
        {
            setPos(_vtx.pos.xyz);
            setColor(_vtx.color);

            nrm[0] = _vtx.nrm.x;    nrm[1] = _vtx.nrm.y;    nrm[2] = _vtx.nrm.z;    // TODO: pack normal
            bin[0] = _vtx.bin.x;    bin[1] = _vtx.bin.y;    bin[2] = _vtx.bin.z;    // TODO: pack binormal
            tan[0] = _vtx.tan.x;    tan[1] = _vtx.tan.y;    tan[2] = _vtx.tan.z;    // TODO: pack tangent

            uv[0][0] = _vtx.uv[0].x;  uv[0][1] = _vtx.uv[0].y;                      // TODO: pack UV0
            uv[1][0] = _vtx.uv[1].x;  uv[1][1] = _vtx.uv[1].y;                      // TODO: pack UV1

            //for (core::uint i = 0; i < 4; ++i)
            //    indices[i] = _vtx.indices[i];
            indices[0] = (_vtx.indices[1] << 16) | _vtx.indices[0];
            indices[1] = (_vtx.indices[3] << 16) | _vtx.indices[2];

            //for (core::uint i = 0; i < 4; ++i)
            //    weights[i] = _vtx.weights[i];
            weights = (_vtx.weights[3] << 24) | (_vtx.weights[2] << 16) | (_vtx.weights[1] << 8) | _vtx.weights[0];
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
        u32         indices[2]; // indices[0] i0 | i1<<16, indicex[1] i2 | i3<<16
        u32         weights;    // w0 | w1<<8 | w2<<16 | w3<<24    
    };

    // VertexFormat enum to VertexFormat struct traits 
    template <> struct VertexStorage<VertexFormat::Skinning_4Bones> { using type = SkinningVertex_4Bones; };
    VG_STATIC_ASSERT(sizeof(SkinningVertex_4Bones) == getVertexFormatStride(VertexFormat::Skinning_4Bones), "SkinningVertex_4Bones struct size does not match HLSL stride");
}

