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

inline float clipZBias(float clip_w,float depth_bias)
{
    float near = 0.1f;
    float far = 1024.0f;

    // calculate near plane depth with bias offset
    float bias_near = near + depth_bias / clip_w;

    // compute scale and translation based on updated near distance
    float bias_scale = far / ( far - bias_near );
    float bias_trans = bias_scale * -bias_near;
    
    // finally, compute and return the generated clip-space z value
    return bias_trans + clip_w * bias_scale;
}

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
    float3 worldPos = mul(float4(modelPos.xyz, 1.0f), rootConstants3D.getWorldMatrix());
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
