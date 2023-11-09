#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/vertex.hlsli"
#include "system/view.hlsli"

#include "editor.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float4 col  : Color;
};

VS_Output VS_DebugDraw(uint _vertexID : VertexID)
{
    VS_Output output = (VS_Output) 0;

    ByteAddressBuffer buf = getBuffer(rootConstants3D.getBufferHandle());
        
    Vertex vert;
           vert.Load(buf, _vertexID, rootConstants3D.getVertexBufferOffset());
    
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    float4x4 viewProj = viewConstants.getViewProj();
    
    output.col = vert.getColor() * rootConstants3D.color;
    float4 modelPos = float4(vert.getPos(), 1.0f);
    float4 worldPos = mul(modelPos, rootConstants3D.mat);
    output.pos = mul(worldPos, viewProj);

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
