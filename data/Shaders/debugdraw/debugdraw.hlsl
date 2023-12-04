#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/vertex.hlsli"
#include "system/view.hlsli"
#include "system/depthbias.hlsli"
#include "debugdraw.hlsli"

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
           vert.Load(buf, rootConstants3D.getVertexFormat(), _vertexID, rootConstants3D.getBufferOffset());
    
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.col = vert.getColor() * rootConstants3D.color;
    float3 modelPos = vert.getPos();
    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), rootConstants3D.getWorldMatrix()).xyz;
    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), view);

    viewPos.z += WIREFRAME_DEPTHBIAS;

    output.pos = mul(viewPos, proj);

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
