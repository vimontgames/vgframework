#include "driver.hlsli"

struct VS_Output_Quad
{
    float4 pos  : Position;
    float2 uv   : Texcoord0;
};

VS_Output_Quad VS_Quad(uint _vertexID : VertexID)
{
    VS_Output_Quad output;

    float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);
    output.uv = tmp;
    output.pos = float4(tmp.xy*2-1, 0, 1);

    return output;
}

struct PS_Output_Quad
{
    float4 color0 : Color0;
};

PS_Output_Quad PS_Quad(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    output.color0 = float4(_input.uv, 0, 1);
    return output;
}



