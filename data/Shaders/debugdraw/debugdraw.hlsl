#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/vertex.hlsli"
#include "system/view.hlsli"
#include "system/depthbias.hlsli"
#include "system/picking.hlsl"
#include "debugdraw.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float3 wpos : WorldPos;
    float4 col  : Color;
};

VS_Output VS_DebugDraw(uint _vertexID : VertexID)
{
    VS_Output output = (VS_Output) 0;

    ByteAddressBuffer buf = getBuffer(debugDrawRootConstants3D.getVertexBufferHandle());
        
    Vertex vert;
           vert.Load(buf, debugDrawRootConstants3D.getVertexFormat(), _vertexID, debugDrawRootConstants3D.getVertexBufferBytesOffset());
    
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.col = vert.getColor() * debugDrawRootConstants3D.getColor();
    float3 modelPos = vert.getPos();

    modelPos.xy *= lerp(1, debugDrawRootConstants3D.getTaper(), modelPos.z*0.5f+0.5f);

    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), debugDrawRootConstants3D.getWorldMatrix()).xyz;

    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), view);
    viewPos.z += WIREFRAME_DEPTHBIAS;

    output.wpos = worldPos;
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

    #if _TOOLMODE && !_ZONLY
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    if (output.color0.a > 0)
    {
        uint toolmodeRWBufferID = viewConstants.getToolmodeRWBufferID();
        if (0xFFFF != toolmodeRWBufferID)
        {
            uint2 inputPos = _input.pos.xy;
            float depth = _input.pos.z;
            uint2 mousePos = viewConstants.getMousePos();
            uint4 pickingID = uint4(debugDrawRootConstants3D.getPickingID(), 0/*rootConstants3D.getMatID()*/, 0, 0);
    
            if (ProcessPicking(toolmodeRWBufferID, 0, inputPos, depth, _input.wpos, mousePos, viewConstants.getScreenSize(), pickingID))
                output.color0 = lerp(output.color0, float4(0,1,0,1), 0.25f);                  
        }
    }

    #endif // #if _TOOLMODE && !_ZONLY

    return output;
}
