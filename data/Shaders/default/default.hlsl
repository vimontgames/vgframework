#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/samplers.hlsli"
#include "system/vertex.hlsli"
#include "system/gamma.hlsli"
#include "system/view.hlsli"

#include "default.hlsli"

struct VS_Output
{
    float4 pos  : Position;
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
           vert.Load(getBuffer(rootConstants3D.getBufferHandle()), _vertexID, rootConstants3D.getVertexBufferOffset());

    output.nrm = vert.getNrm();
    output.tan = vert.getTan();
    output.bin = vert.getBin();
    output.uv = float4(vert.getUV(0), vert.getUV(1));
    output.col = vert.getColor() * rootConstants3D.color;
    output.pos = mul(float4(vert.getPos(), 1.0f), rootConstants3D.mat);

    return output;
}

struct PS_Output
{
    float4 color0 : Color0;
};

float3 getMatIDColor(uint _matID)
{
    switch (_matID % 6)
    {
        case 0: return float3(1, 0, 0);
        case 1: return float3(0, 1, 0);
        case 2: return float3(1, 1, 0);
        case 3: return float3(0, 0, 1);
        case 4: return float3(1, 0, 1);
        case 5: return float3(0, 1, 1);
    }

    return 0;
}

float safeLength(float2 a, float2 b)
{
    float2 diff = a - b;
    return all(0 != diff)? length(diff) : 0;
}

PS_Output PS_Forward(VS_Output _input)
{
    PS_Output output;
    
    // test ReservedConstants slots
    ViewConstants viewConstants;
    viewConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_VIEWCONSTANTS));
            
    // test dynamic buffer
    //float4 test = LoadF4(getBuffer(16388), 0);
    //output.color0 = test;
    //return output;
    
    float2 uv0 = _input.uv.xy;
    float2 uv1 = _input.uv.zw;
    
    float4 albedo = getTexture2D( rootConstants3D.getAlbedoTextureHandle() ).Sample(linearRepeat, uv0).rgba;
    float3 normal = getTexture2D( rootConstants3D.getNormalTextureHandle() ).Sample(linearRepeat, uv0).rgb*2-1;

    #if _TOOLMODE
    DisplayFlags flags = viewConstants.getDisplayFlags();
    if (0 == (DisplayFlags::AlbedoMap & flags))
        albedo = pow(0.5, 0.45);

    if (0 == (DisplayFlags::NormalMap & flags))
        normal = float3(0,0,1);
    #endif

    float3 T = normalize(_input.tan);
    float3 B = normalize(_input.bin);
    float3 N = normalize(_input.nrm);

    float3 worldNormal = normalize(T * normal.x + B * normal.y + N * normal.z);

    // fake shitty lighting
    float3 fakeDiffuseLighting = saturate(dot(worldNormal, normalize(float3(-1, -1, 2))) * 0.8f);
    float3 fakeAmbientLighting = 0.1f;

    output.color0.rgba = float4(albedo.rgb * (fakeDiffuseLighting + fakeAmbientLighting), 1.0f) * _input.col;

    #if _TOOLMODE
    DisplayMode mode = viewConstants.getDisplayMode();
    switch (mode)
    {
        case DisplayMode::Default:
            break;
    
        case DisplayMode::MatID:
            output.color0 = sRGBA2Linear(float4(getMatIDColor(rootConstants3D.getMatID()), 1.0f));
            break;
    
        case DisplayMode::VSNormal:
            output.color0 = sRGBA2Linear(float4(N * 0.5f + 0.5f, 1.0f));
            break;
    
        case DisplayMode::VSTangent:
            output.color0 = sRGBA2Linear(float4(T * 0.5f + 0.5f, 1.0f));
            break;
    
        case DisplayMode::VSBinormal:
            output.color0 = sRGBA2Linear(float4(B * 0.5f + 0.5f, 1.0f));
            break;

        case DisplayMode::VSColor:
            output.color0 = _input.col;
            break;
    
        case DisplayMode::UV0:
            output.color0 = sRGBA2Linear(float4(uv0.xy, 0, 1));
            break;
    
        case DisplayMode::UV1:
            output.color0 = sRGBA2Linear(float4(uv1.xy, 0, 1));
            break;
    
        case DisplayMode::Albedo:
            output.color0 = float4(albedo.rgb, 1);
            break;
    
        case DisplayMode::PSNormal:
            output.color0 = sRGBA2Linear(float4(normal.rgb * 0.5 + 0.5, 1));
            break;
    }
    #endif // _TOOLMODE
    
    #if _EDITOR
    // TODO: select editor pass according to root constant flags
    if (RootConstantsEditorFlags::Wireframe & rootConstants3D.getFlags())
        output.color0 = float4(0,1,0,1);
    #endif
    
    // Picking test
    float2 screenSize = viewConstants.getScreenSize();
    float2 screenPos = _input.pos.xy / screenSize;
    //output.color0 = float4(frac(16 * screenPos), 0, 1);
    //return output;
    
    uint2 mousePos = viewConstants.getMousePos();
    float dist = safeLength(mousePos, _input.pos.xy);
    if (all(dist < 4))
    {
        output.color0.rgb = lerp(output.color0.rgb, float3(0, 1, 0), 0.25f);
        return output;
    }
            
    return output;
}