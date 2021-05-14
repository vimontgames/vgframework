#include "../system/bindless.hlsli"
#include "../system/semantics.hlsli"
#include "../system/samplers.hlsli"
#include "../system/vertex.hlsli"

#include "default.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float3 nrm  : Normal;
    float2 uv   : Texcoord0;
};

VS_Output VS_Forward(uint _vertexID : VertexID)
{
    VS_Output output;

    Vertex vert;
           vert.load(getBuffer(rootConstants3D.getBuffer()), _vertexID);

    output.nrm = vert.getNrm();
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
    
    float4 baseColor = Texture2DTable[rootConstants3D.getTexture()].Sample(nearestRepeat, uv).rgba;

    // fake shitty lighting
    float3 fakeDiffuseLighting = dot(_input.nrm, -normalize(float3(-1,-1,-1))) * 0.9f;
    float3 fakeAmbientLighting = 0.3f;

    output.color0.rgba = float4( baseColor.rgb * (fakeDiffuseLighting + fakeAmbientLighting), 1.0f);

    switch (rootConstants3D.getMode())
    {
        case MODE_MATID:
            output.color0 = float4(getMatIDColor(rootConstants3D.getMatID()), 1.0f);
            break;
        case MODE_NORMAL:
            output.color0 = float4(_input.nrm*0.5f + 0.5f, 1.0f);
            break;
        case MODE_UV0:
            output.color0 = float4(uv, 0, 1);
            break;
    }
    
    return output;
}

PS_Output PS_Wireframe(VS_Output _input)
{
    PS_Output output;

    output.color0 = float4(0, 1, 0, 1);
    return output;
}
