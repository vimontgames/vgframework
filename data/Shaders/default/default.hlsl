#include "../system/bindless.hlsli"
#include "../system/semantics.hlsli"
#include "../system/samplers.hlsli"
#include "../system/vertex.hlsli"

#include "default.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float3 nrm  : Normal;
    float3 tan  : Tangent;
    float3 bin  : Binormal;
    float2 uv   : Texcoord0;
};

VS_Output VS_Forward(uint _vertexID : VertexID)
{
    VS_Output output;

    Vertex vert;
           vert.load(getBuffer(rootConstants3D.getBuffer()), _vertexID, rootConstants3D.getVertexBufferOffset());

    output.nrm = vert.getNrm();
    output.tan = vert.getTan();
    output.bin = vert.getBin();
    output.uv = vert.getUV(0);
    output.pos = mul(float4(vert.getPos(), 1.0f), rootConstants3D.mat);

    return output;
}

struct PS_Output
{
    float4 color0 : Color0;
};

float3 getMatIDColor(uint _matID)
{
    switch (_matID % 6)
    {
        case 0: return float3(1, 0, 0);
        case 1: return float3(0, 1, 0);
        case 2: return float3(1, 1, 0);
        case 3: return float3(0, 0, 1);
        case 4: return float3(1, 0, 1);
        case 5: return float3(0, 1, 1);
    }

    return 0;
}

PS_Output PS_Forward(VS_Output _input)
{
    PS_Output output;
    float2 uv = _input.uv;
    
    float4 albedo = Texture2DTable[rootConstants3D.getAlbedoMap()].Sample(linearRepeat, uv).rgba;
    float3 normal = Texture2DTable[rootConstants3D.getNormalMap()].Sample(linearRepeat, uv).rgb*2-1;

    if (0 == (rootConstants3D.getFlags() & FLAG_ALBEDOMAPS))
        albedo = pow(0.5, 0.45);

    if (0 == (rootConstants3D.getFlags() & FLAG_NORMALMAPS))
        normal = float3(0,0,1);

#if 1
    float3 T = normalize(_input.tan);
    float3 B = normalize(_input.bin);
    float3 N = normalize(_input.nrm);
#else
    float3 pos = _input.pos.xyz / _input.pos.w;
    float3 q0 = ddx(pos);
    float3 q1 = ddy(pos);
    float2 st0 = ddx(uv.xy);
    float2 st1 = ddy(uv.xy);
    
    float3 T = normalize(-q0 * st1.x + q1 * st0.x);
    float3 B = normalize( q0 * st1.y - q1 * st0.y);
    float3 N = _input.nrm;
#endif

    float3 worldNormal = normalize(T * normal.x + B * normal.y + N * normal.z);

    // fake shitty lighting
    float3 fakeDiffuseLighting = dot(worldNormal, normalize(float3(-1, -2, 3))) * 0.5f;
    float3 fakeAmbientLighting = 0.5f;

    output.color0.rgba = float4(albedo.rgb * (fakeDiffuseLighting + fakeAmbientLighting), 1.0f);

    switch (rootConstants3D.getMode())
    {
        case MODE_MATID:
            output.color0 = float4(getMatIDColor(rootConstants3D.getMatID()), 1.0f);
            break;
        case MODE_VS_TANGENT:
            output.color0 = float4(T*0.5f + 0.5f, 1.0f);
            break;
        case MODE_VS_BINORMAL:
            output.color0 = float4(B*0.5f + 0.5f, 1.0f);
            break;
        case MODE_VS_NORMAL:
            output.color0 = float4(N*0.5f + 0.5f, 1.0f);
            break;
        case MODE_UV0:
            output.color0 = float4(uv, 0, 1);
            break;
        case MODE_ALBEDO_MAP:
            output.color0 = float4(albedo.rgb, 1);
            break;
        case MODE_NORMAL_MAP:
            output.color0 = float4(normal.rgb*0.5+0.5, 1);
            break;
    }
    
    return output;
}

PS_Output PS_Wireframe(VS_Output _input)
{
    PS_Output output;
    output.color0 = rootConstants3D.getWireframeColor();
    return output;
}
