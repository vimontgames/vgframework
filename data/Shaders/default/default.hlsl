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
#include "system/instancedata.hlsli"

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

//--------------------------------------------------------------------------------------
VS_Output VS_Forward(uint _vertexID : VertexID)
{
    VS_Output output;

    uint instanceDataOffset = rootConstants3D.getGPUInstanceDataOffset(); 
    GPUInstanceData instanceData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUInstanceData>(instanceDataOffset);
    GPUMaterialData materialData = instanceData.loadGPUMaterialData(instanceDataOffset, rootConstants3D.getMatID());

    Vertex vert;
           vert.Load(getBuffer(rootConstants3D.getVertexBufferHandle()), rootConstants3D.getVertexFormat(), _vertexID, rootConstants3D.getVertexBufferOffset());
           //vert.Load(getBuffer(instanceData.getVertexBufferHandle()), instanceData.getVertexFormat(), _vertexID, instanceData.getVertexBufferOffset());

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    DisplayFlags flags = viewConstants.getDisplayFlags();

    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.nrm = vert.getNrm();
    output.tan = vert.getTan();
    output.bin = vert.getBin();
    output.uv = float4(vert.getUV(0), vert.getUV(1));
    output.col = materialData.getVertexColorOut(vert.getColor(), instanceData.getInstanceColor(), flags);
    
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

//--------------------------------------------------------------------------------------
struct PS_Output
{
    float4 color0 : Color0;
};

//--------------------------------------------------------------------------------------
float4 getNormal(GPUMaterialData _materialData, float2 _uv, DisplayFlags _flags)
{
    float4 normal = getTexture2D(_materialData.getNormalTextureHandle()).Sample(linearRepeat, _uv);

    #if _TOOLMODE
    if (0 == (DisplayFlags::NormalMap & _flags))
        normal.xyz = float3(0.5, 0.5, 1.0);
    #endif

    return float4((normal.xy*2-1) * _materialData.getNormalStrength(), normal.z, normal.w);
}

//--------------------------------------------------------------------------------------
float4 getPBR(GPUMaterialData _materialData, float2 _uv)
{
    float4 pbr = getTexture2D(_materialData.getPBRTextureHandle()).Sample(linearRepeat, _uv);

    pbr.r = lerp(1.0f, pbr.r, _materialData.getOcclusion());
    pbr.g *= _materialData.getRoughness();
    pbr.b *= _materialData.getMetalness();

    //pbr.r = 1;
    //pbr.g = 0.3f;
    //pbr.b = 0;

    return pbr;
}

//--------------------------------------------------------------------------------------
// TODO: move to HLSL math header?
//--------------------------------------------------------------------------------------
float4x4 getMatrixWithoutScale(float4x4 _matrix)
{
    float4x4 matrix = _matrix;
    matrix[0].xyz = matrix[0].xyz / length(_matrix[0].xyz);
    matrix[1].xyz = matrix[1].xyz / length(_matrix[1].xyz);
    matrix[2].xyz = matrix[2].xyz / length(_matrix[2].xyz);
    return matrix;
}

//--------------------------------------------------------------------------------------
float3 getWorldNormal(float3 _normal, float3 _vertexTangent, float3 _vertexBinormal, float3 _vertexNormal, float4x4 _world)
{
    float3 T = normalize(_vertexTangent);
    float3 B = normalize(_vertexBinormal);
    float3 N = normalize(_vertexNormal);

    float3 worldNormal = normalize(T * _normal.x + B * _normal.y + N * _normal.z);
           worldNormal = mul(float4(worldNormal.xyz, 0.0f), getMatrixWithoutScale(_world)).xyz;

    return worldNormal;
}



//--------------------------------------------------------------------------------------
#if _ZONLY
void PS_Forward(VS_Output _input)
#else
PS_Output PS_Forward(VS_Output _input)
#endif
{
    PS_Output output = (PS_Output)0;
    
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    DisplayFlags flags = viewConstants.getDisplayFlags();
    
    uint2 screenSize = viewConstants.getScreenSize();
    float3 screenPos = _input.pos.xyz / float3(screenSize.xy, 1);
    float3 worldPos = _input.wpos.xyz;
    float3 camPos = viewConstants.getCameraPos();  

    // Get texture handles from instance data. If no instance data specified, offset is 0 and default instance data is used
    uint instanceDataOffset = rootConstants3D.getGPUInstanceDataOffset(); 
    GPUInstanceData instanceData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUInstanceData>(instanceDataOffset);

    // Material entries are only present once material is loaded, if matIndex is above limit then use the default material
    GPUMaterialData materialData = instanceData.loadGPUMaterialData(instanceDataOffset, rootConstants3D.getMatID());

    float2 uv0 = materialData.GetUV0(_input.uv.xy,_input.uv.zw, worldPos);
    float2 uv1 = _input.uv.zw;

    float4 albedo = materialData.getAlbedo(uv0, _input.col, flags);
    float3 normal = getNormal(materialData, uv0, flags).xyz;
    float4 pbr = getPBR(materialData, uv0);

    float3 worldNormal = getWorldNormal(normal, _input.tan, _input.bin, _input.nrm, rootConstants3D.getWorldMatrix());

    // Compute & Apply lighting
    LightingResult lighting = computeDirectLighting(viewConstants, camPos, worldPos, albedo.rgb, worldNormal.xyz, pbr);

    output.color0.rgb = applyLighting(albedo.rgb, lighting, viewConstants.getDisplayMode());
    output.color0.a = albedo.a; 
    
    #if _TOOLMODE && !_ZONLY

    output.color0 = forwardDebugDisplay(output.color0, viewConstants.getDisplayMode(), rootConstants3D.getMatID(), _input.tan.xyz, _input.bin.xyz, _input.nrm.xyz, _input.col, uv0, uv1, screenPos.xy, worldPos.xyz, albedo.rgb, normal.xyz, worldNormal.xyz, pbr.rgb);

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
    
    #if !_ALPHABLEND
    output.color0.a = 1.0f;           
    #endif 

    #if _ZONLY
    return;
    #else
    return output;
    #endif
}

//--------------------------------------------------------------------------------------
VS_Output VS_Deferred(uint _vertexID : VertexID)
{
    VS_Output output;

    uint instanceDataOffset = rootConstants3D.getGPUInstanceDataOffset(); 
    GPUInstanceData instanceData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUInstanceData>(instanceDataOffset);
    GPUMaterialData materialData = instanceData.loadGPUMaterialData(instanceDataOffset, rootConstants3D.getMatID());

    Vertex vert;
           vert.Load(getBuffer(rootConstants3D.getVertexBufferHandle()), rootConstants3D.getVertexFormat(), _vertexID, rootConstants3D.getVertexBufferOffset());

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    DisplayFlags flags = viewConstants.getDisplayFlags();

    float4x4 view = viewConstants.getView();
    float4x4 proj = viewConstants.getProj();
    
    output.nrm = vert.getNrm();
    output.tan = vert.getTan();
    output.bin = vert.getBin();
    output.uv = float4(vert.getUV(0), vert.getUV(1));
    output.col = materialData.getVertexColorOut(vert.getColor(), instanceData.getInstanceColor(), flags);
    
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

//--------------------------------------------------------------------------------------
struct PS_OutputDeferred
{
    float4 albedo   : Color0;
    float4 normal   : Color1;
    float4 pbr      : Color2;
};

//--------------------------------------------------------------------------------------
#if _ZONLY
void PS_Deferred(VS_Output _input)
#else
PS_OutputDeferred PS_Deferred(VS_Output _input)
#endif
{
    PS_OutputDeferred output = (PS_OutputDeferred)0;

    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));

    DisplayFlags flags = viewConstants.getDisplayFlags();

    uint2 screenSize = viewConstants.getScreenSize();
    float3 screenPos = _input.pos.xyz / float3(screenSize.xy, 1);
    float3 worldPos = _input.wpos.xyz;        

    // Get texture handles from instance data. If no instance data specified, offset is 0 and default instance data is used
    uint instanceDataOffset = rootConstants3D.getGPUInstanceDataOffset(); 
    GPUInstanceData instanceData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUInstanceData>(instanceDataOffset);

    // Material entries are only present once material is loaded, if matIndex is above limit then use the default material
    GPUMaterialData materialData = instanceData.loadGPUMaterialData(instanceDataOffset, rootConstants3D.getMatID());

    float2 uv0 = materialData.GetUV0(_input.uv.xy,_input.uv.zw, worldPos);
    float2 uv1 = _input.uv.zw;

    float4 albedo = materialData.getAlbedo(uv0, _input.col, flags);
    float3 normal = getNormal(materialData, uv0, flags).xyz;
    float4 pbr = getPBR(materialData, uv0);
    
    float3 worldNormal = getWorldNormal(normal, _input.tan, _input.bin, _input.nrm, rootConstants3D.getWorldMatrix());

    output.albedo = albedo.rgba;
    output.normal = float4(worldNormal.xyz, 0.0);
    output.pbr = float4(pbr.rgb, 0.0);

    #if _TOOLMODE && !_ZONLY
    // If any 'Forward' debug display mode is enabled then its result is stored into the 'Albedo' buffer
    output.albedo = forwardDebugDisplay(output.albedo, viewConstants.getDisplayMode(), rootConstants3D.getMatID(), _input.tan.xyz, _input.bin.xyz, _input.nrm.xyz, _input.col, uv0, uv1, screenPos.xy, worldPos.xyz, albedo.rgb, normal.xyz, worldNormal.xyz, pbr.rgb);

    // Picking
    uint toolmodeRWBufferID = viewConstants.getToolmodeRWBufferID();
    if (0xFFFF != toolmodeRWBufferID)
    {
        uint2 inputPos = _input.pos.xy;
        float depth = _input.pos.z;
        uint2 mousePos = viewConstants.getMousePos();
        uint4 pickingID = uint4(rootConstants3D.getPickingID(), rootConstants3D.getMatID(), 0, 0);
    
        if (ProcessPicking(toolmodeRWBufferID, 0, inputPos, depth, worldPos, mousePos, screenSize, pickingID))
            output.albedo = lerp(output.albedo, float4(0,1,0,1), 0.25);
    }
    #endif // _TOOLMODE && !_ZONLY

    #if _ALPHATEST
    clip(output.albedo.a - 0.5); 
    #endif

    #if !_ALPHABLEND
    output.albedo.a = 1;
    #endif

    #if _ZONLY
    return;
    #else    
    return output;
    #endif
}