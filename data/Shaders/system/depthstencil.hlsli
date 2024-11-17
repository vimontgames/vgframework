#ifndef _DEPTHSTENCIL__HLSLI_
#define _DEPTHSTENCIL__HLSLI_

//--------------------------------------------------------------------------------------
// Load Depth/Stencil components
//--------------------------------------------------------------------------------------
#define loadDepth(depthTex, address) getTexture2D(depthTex).Load(address).r;
        
#ifdef _VULKAN
#define loadStencil(stencilTex, address) getTexture2D_UInt2(stencilTex).Load(address).r;
#else
#define loadStencil(stencilTex, address) getTexture2D_UInt2(stencilTex).Load(address).g;
#endif

#endif // _DRIVER__HLSLI_