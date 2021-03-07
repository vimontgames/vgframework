#include "../system/semantics.hlsli"
#include "background.hlsli"

struct VS_Output_Quad
{
    float4 pos  : Position;
    float2 uv   : Texcoord0;
};

VS_Output_Quad VS_Background(uint _vertexID : VertexID)
{
    VS_Output_Quad output;

    output.uv = rootConstants.quad.getUV0(_vertexID);
    output.pos = rootConstants.quad.getPosition(_vertexID);

    return output;
}

struct PS_Output_Quad
{
    float4 color0 : Color0;
};

PS_Output_Quad PS_Gradient(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    float2 uv = _input.uv;
    output.color0 = rootConstants.color;
    return output;
}



