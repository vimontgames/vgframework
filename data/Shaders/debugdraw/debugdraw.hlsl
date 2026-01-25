#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/vertex.hlsli"
#include "system/view.hlsli"
#include "system/depthbias.hlsli"
#include "system/picking.hlsl"
#include "system/outlinemask.hlsli"
#include "system/transparency.hlsli"
#include "debugdraw.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float3 wpos : WorldPos;
    float3 nrm : Normal;
    float4 col  : Color;
};

VS_Output VS_DebugDraw(uint _vertexID : VertexID)
{
    VS_Output output = (VS_Output) 0;

    ByteAddressBuffer buf = getBuffer(debugDrawRootConstants3D.getVertexBufferHandle());
        
    Vertex vert;
           vert.Load(buf, debugDrawRootConstants3D.getVertexFormat(), _vertexID, debugDrawRootConstants3D.getVertexBufferOffset());
    
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.col = vert.getColor() * debugDrawRootConstants3D.getColor();
    float3 modelPos = vert.getPos();

    modelPos.xy *= lerp(1, debugDrawRootConstants3D.getTaper(), modelPos.z*0.5f+0.5f);
    
    float4x4 world = debugDrawRootConstants3D.getWorldMatrix();
    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), world).xyz;

    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), view);
    viewPos.z += WIREFRAME_DEPTHBIAS;

    output.wpos = worldPos;
    output.pos = mul(viewPos, proj);

    output.nrm = mul(float4(vert.getNrm(), 0.0f), clearScale(world)).xyz;

    return output;
}

struct PS_Output
{
    float4 color0 : Color0;
};

PS_Output PS_DebugDraw(VS_Output _input)
{
    PS_Output output;
     
    if (debugDrawRootConstants3D.getVertexFormat() == VertexFormat::DebugDrawLit)
    {
        output.color0 = float4( (saturate(dot(_input.nrm, normalize(float3(1,2,3))).xxx * 0.25) + 0.75) * _input.col.rgb, _input.col.a); 
    }
    else
    {
        output.color0 = _input.col.rgba; 
    }

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

//--------------------------------------------------------------------------------------
VS_Output_Outline VS_DebugDrawOutline(uint _vertexID : VertexID)
{
    VS_Output_Outline output;

    Vertex vert;
           vert.Load(getBuffer(debugDrawRootConstants3D.getVertexBufferHandle()), debugDrawRootConstants3D.getVertexFormat(), _vertexID, debugDrawRootConstants3D.getVertexBufferOffset());

    ViewConstants viewConstants;
                  viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
        
    float3 modelPos = vert.getPos();
    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), debugDrawRootConstants3D.getWorldMatrix()).xyz;
    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), view); 

    viewPos.z += WIREFRAME_DEPTHBIAS;
    output.vpos = viewPos;
    output.pos = mul(viewPos, proj);

    return output;
}

PS_Output_Outline PS_DebugDrawOutline(VS_Output_Outline _input)
{
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    
    uint2 screenSize = viewConstants.getScreenSize();
    float3 screenPos = _input.pos.xyz / float3(screenSize.xy, 1);

    PS_Output_Outline output = (PS_Output_Outline)0;
    uint id = debugDrawRootConstants3D.getPickingID();
    uint flags = 0x0;
    if (!linearDepthTest(screenPos.xy, _input.vpos))
        flags |= (uint)OutlineMaskFlags::DepthFail;

    output.value.x = id;
    output.value.y = flags;
    
    return output;
}
