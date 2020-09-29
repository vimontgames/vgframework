#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

#ifndef __cplusplus
Texture1D Texture1DTable[] : register(t0, space0);
Texture2D Texture2DTable[] : register(t0, space0);
Texture3D Texture3DTable[] : register(t0, space0);
#endif

#endif // _DRIVER__HLSLI_