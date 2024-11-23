#ifndef _DEPTHSTENCIL__HLSLI_
#define _DEPTHSTENCIL__HLSLI_

//--------------------------------------------------------------------------------------
// Load Depth/Stencil components
//--------------------------------------------------------------------------------------
#define loadDepth(depthTex, address) getTexture2D(depthTex).Load(int3(address.xy,0)).r
#define loadDepthMSAA(depthTex, address, sampleIndex) getTexture2DMS(depthTex).Load(address.xy, sampleIndex).r
        
#ifdef _VULKAN
#define loadStencil(stencilTex, address) getTexture2D_UInt2(stencilTex).Load(int3(address.xy,0)).r
#define loadStencilMSAA(stencilTex, address, sampleIndex) getTexture2DMS_UInt2(stencilTex).Load(address.xy, sampleIndex).r
#else
#define loadStencil(stencilTex, address) getTexture2D_UInt2(stencilTex).Load(int3(address.xy, 0)).g
#define loadStencilMSAA(stencilTex, address, sampleIndex) getTexture2DMS_UInt2(stencilTex).Load(address.xy, sampleIndex).g
#endif

#endif // _DRIVER__HLSLI_