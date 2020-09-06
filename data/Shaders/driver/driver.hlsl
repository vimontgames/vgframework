#include "driver.hlsli"

struct VS_Input_Quad
{
    uint vertexID : VertexID;
};

struct VS_Output_Quad
{
    float4 pos : Position;
};

VS_Output_Quad VS_Quad(VS_Input_Quad _input)
{
    VS_Output_Quad output;

    float2 pos = float2(_input.vertexID & 1, (_input.vertexID & 2) >> 1);
    output.pos = float4(pos.xy, 0, 1);

    return output;
}

struct PS_Output_Quad
{
    float4 color0 : Color0;
};

PS_Output_Quad PS_Quad(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    output.color0 = float4(1, 1, 0, 1);
    return output;
}

