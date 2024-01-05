#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/samplers.hlsli"
#include "system/vertex.hlsli"
#include "system/gamma.hlsli"
#include "system/view.hlsli"
#include "system/depthbias.hlsli"
#include "system/picking.hlsl"
#include "system/lighting.hlsli"
#include "system/debugdisplay.hlsli"

#include "default.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float3 wpos : WorldPos;
    float3 nrm  : Normal;
    float3 tan  : Tangent;
    float3 bin  : Binormal;
    float4 uv   : Texcoord0;
    float4 col  : Color;
};

VS_Output VS_Forward(uint _vertexID : VertexID)
{
    VS_Output output;

    Vertex vert;
           vert.Load(getBuffer(rootConstants3D.getBufferHandle()), rootConstants3D.getVertexFormat(), _vertexID, rootConstants3D.getBufferOffset());

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.nrm = vert.getNrm();
    output.tan = vert.getTan();
    output.bin = vert.getBin();
    output.uv = float4(vert.getUV(0), vert.getUV(1));
    output.col = vert.getColor() * rootConstants3D.color;
    
    float3 modelPos = vert.getPos();
    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), rootConstants3D.getWorldMatrix()).xyz;
    output.wpos = worldPos.xyz;
    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), view);

    #ifdef _TOOLMODE
    if (RootConstantsFlags::Wireframe & rootConstants3D.getFlags())
        viewPos.z += WIREFRAME_DEPTHBIAS;
    #endif

    output.pos = mul(viewPos, proj);

    return output;
}

struct PS_Output
{
    float4 color0 : Color0;
};

float4 getAlbedo(float2 _uv, float4 _vertexColor, DisplayFlags _flags)
{
    float4 albedo = getTexture2D( rootConstants3D.getAlbedoTextureHandle() ).Sample(linearRepeat, _uv).rgba;

    #if _TOOLMODE
    if (0 == (DisplayFlags::AlbedoMap & _flags))
        albedo = pow(0.5, 0.45);
    #endif

    albedo *= _vertexColor;
    
    return albedo;
}

float4 getNormal(float2 _uv, DisplayFlags _flags)
{
    float4 normal = getTexture2D( rootConstants3D.getNormalTextureHandle() ).Sample(linearRepeat, _uv);

    #if _TOOLMODE
    if (0 == (DisplayFlags::NormalMap & _flags))
        normal.xyz = float3(0,0,1);
    #endif

    return float4(normal.xyz*2-1, normal.w);
}

float4 getPBR(float2 _uv)
{
    float4 pbr = getTexture2D(rootConstants3D.getPBRTextureHandle() ).Sample(linearRepeat, _uv);
    return pbr;
}

float3 getWorldNormal(float3 _normal, float3 _vertexTangent, float3 _vertexBinormal, float3 _vertexNormal, float4x4 _world)
{
    float3 T = normalize(_vertexTangent);
    float3 B = normalize(_vertexBinormal);
    float3 N = normalize(_vertexNormal);

    float3 worldNormal = normalize(T * _normal.x + B * _normal.y + N * _normal.z);
           worldNormal = mul(float4(worldNormal.xyz, 0.0f), _world).xyz;

    return worldNormal;
}

PS_Output PS_Forward(VS_Output _input)
{
    PS_Output output = (PS_Output)0;

    //output.color0 = float4(frac(_input.pos.xyz*100),1);
    //return output;
    
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    DisplayFlags flags = viewConstants.getDisplayFlags();
    
    uint2 screenSize = viewConstants.getScreenSize();
    float3 screenPos = _input.pos.xyz / float3(screenSize.xy, 1);
    float3 worldPos = _input.wpos.xyz;
    float3 camPos = viewConstants.getCameraPos();  

    float2 uv0 = _input.uv.xy;
    float2 uv1 = _input.uv.zw;
    
    float4 albedo = getAlbedo(uv0, _input.col, flags);
    float3 normal = getNormal(uv0, flags).xyz;
    float4 pbr = getPBR(uv0);
    
    float3 worldNormal = getWorldNormal(normal, _input.tan, _input.bin, _input.nrm, rootConstants3D.getWorldMatrix());

    // Compute & Apply lighting
    LightingResult lighting = computeDirectLighting(viewConstants, camPos, worldPos, albedo.rgb, worldNormal.xyz, pbr);

    output.color0.rgb = applyLighting(albedo.rgb, lighting, viewConstants.getDisplayMode());
    output.color0.a = albedo.a; 
    
    #if _TOOLMODE && !_ZONLY

    output.color0 = forwardDebugDisplay(output.color0, viewConstants.getDisplayMode(), rootConstants3D.getMatID(), _input.tan.xyz, _input.bin.xyz, _input.nrm.xyz, _input.col, uv0, uv1, screenPos.xy, worldPos.xyz, albedo.rgb, normal.xyz);
    
    if (RootConstantsFlags::Wireframe & rootConstants3D.getFlags())
        output.color0 = float4(0,1,0,1);

    // Picking
    uint toolmodeRWBufferID = viewConstants.getToolmodeRWBufferID();
    if (0xFFFF != toolmodeRWBufferID)
    {
        uint2 inputPos = _input.pos.xy;
        float depth = _input.pos.z;
        uint2 mousePos = viewConstants.getMousePos();
        uint4 pickingID = uint4(rootConstants3D.getPickingID(), rootConstants3D.getMatID(), 0, 0);
    
        if (ProcessPicking(toolmodeRWBufferID, 0, inputPos, depth, worldPos, mousePos, screenSize, pickingID))
            output.color0 = lerp(output.color0, float4(0,1,0,1), 0.25f);
    }

    #endif // _TOOLMODE

    #if _ALPHATEST
    clip(output.color0.a-0.5f); 
    #endif

    #if _ZONLY
    output.color0 = (float4)0.0f;
    #endif
    
    #if !_ALPHABLEND
    output.color0.a = 1.0f;           
    #endif 

    return output;
}

VS_Output VS_Deferred(uint _vertexID : VertexID)
{
    VS_Output output;

    Vertex vert;
           vert.Load(getBuffer(rootConstants3D.getBufferHandle()), rootConstants3D.getVertexFormat(), _vertexID, rootConstants3D.getBufferOffset());

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.nrm = vert.getNrm();
    output.tan = vert.getTan();
    output.bin = vert.getBin();
    output.uv = float4(vert.getUV(0), vert.getUV(1));
    output.col = vert.getColor() * rootConstants3D.color;
    
    float3 modelPos = vert.getPos();
    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), rootConstants3D.getWorldMatrix()).xyz;
    output.wpos = worldPos.xyz;
    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), view);

    #ifdef _TOOLMODE
    if (RootConstantsFlags::Wireframe & rootConstants3D.getFlags())
        viewPos.z += WIREFRAME_DEPTHBIAS;
    #endif

    output.pos = mul(viewPos, proj);

    return output;
}

struct PS_OutputDeferred
{
    float4 albedo   : Color0;
    float4 normal   : Color1;
    float4 pbr      : Color2;
};

PS_OutputDeferred PS_Deferred(VS_Output _input)
{
    PS_OutputDeferred output = (PS_OutputDeferred)0;

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    DisplayFlags flags = viewConstants.getDisplayFlags();

    uint2 screenSize = viewConstants.getScreenSize();
    float3 screenPos = _input.pos.xyz / float3(screenSize.xy, 1);
    float3 worldPos = _input.wpos.xyz;        

    float2 uv0 = _input.uv.xy;
    float2 uv1 = _input.uv.zw;
    
    float4 albedo = getAlbedo(uv0, _input.col, flags);
    float3 normal = getNormal(uv0, flags).xyz;
    float4 pbr = getPBR(uv0);
    
    float3 worldNormal = getWorldNormal(normal, _input.tan, _input.bin, _input.nrm, rootConstants3D.getWorldMatrix());

    output.albedo = float4(albedo.rgb, albedo.a);
    output.normal = float4(worldNormal.xyz, 1.0f);
    output.pbr = pbr;

    #if _TOOLMODE && !_ZONLY
    // If any 'Forward' debug display mode is enabled then its result is stored into the 'Albedo' buffer
    output.albedo = forwardDebugDisplay(output.albedo, viewConstants.getDisplayMode(), rootConstants3D.getMatID(), _input.tan.xyz, _input.bin.xyz, _input.nrm.xyz, _input.col, uv0, uv1, screenPos.xy, worldPos.xyz, albedo.rgb, normal.xyz);

    // Picking
    uint toolmodeRWBufferID = viewConstants.getToolmodeRWBufferID();
    if (0xFFFF != toolmodeRWBufferID)
    {
        uint2 inputPos = _input.pos.xy;
        float depth = _input.pos.z;
        uint2 mousePos = viewConstants.getMousePos();
        uint4 pickingID = uint4(rootConstants3D.getPickingID(), rootConstants3D.getMatID(), 0, 0);
    
        if (ProcessPicking(toolmodeRWBufferID, 0, inputPos, depth, worldPos, mousePos, screenSize, pickingID))
            output.albedo = lerp(output.albedo, float4(0,1,0,1), 0.25f);
    }
    #endif // _TOOLMODE && !_ZONLY

    #if _ALPHATEST
    clip(output.albedo.a-0.5f); 
    #endif

    #if _ZONLY
    output = (PS_OutputDeferred)0;
    #endif
    
    return output;
}