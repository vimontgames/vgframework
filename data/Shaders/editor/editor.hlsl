#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/vertex.hlsli"

#include "editor.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float4 col  : Color;
};

VS_Output VS_DebugDraw(uint _vertexID : VertexID)
{
    VS_Output output;

    Vertex vert;
           vert.Load(getBuffer(rootConstants3D.getBufferHandle()), _vertexID, rootConstants3D.getVertexBufferOffset());

    output.col = vert.getColor() * rootConstants3D.color;
    output.pos = mul(float4(vert.getPos(), 1.0f), rootConstants3D.mat);

    return output;
}

struct PS_Output
{
    float4 color0 : Color0;
};

PS_Output PS_DebugDraw(VS_Output _input)
{
    PS_Output output;
    output.color0 = _input.col;
    return output;
}
