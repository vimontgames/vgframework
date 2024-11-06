#ifndef _ENVIRONMENT__HLSLI_
#define _ENVIRONMENT__HLSLI_

float3 getEnvironmentBackgroundColor(float2 _uv, ViewConstants _viewConstants)
{
    uint ambientCubemapTexHandle = _viewConstants.getEnvironmentTextureHandle();
    
    if (ReservedSlot::InvalidTextureCube != (ReservedSlot)ambientCubemapTexHandle)
    {
        float4 clipPos = float4(_uv*2-1, 1,1);
        float4 viewPos = mul(clipPos, _viewConstants.getProjInv());
                viewPos.xz *= -1;
        float4 worldPos = mul(viewPos, _viewConstants.getViewInv());
                worldPos.xyz *= -1;
    
	    TextureCube cubemap = getTextureCube(ambientCubemapTexHandle);        
        return cubemap.SampleLevel(linearClamp, normalize(worldPos.xyz), 0).rgb;      
    }
    else
    {
        float3 envColor = _viewConstants.getEnvironmentColor();
        return envColor; 
    }      
}

#endif // _ENVIRONMENT__HLSLI_