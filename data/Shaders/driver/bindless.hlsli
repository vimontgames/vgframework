#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

//#ifdef VULKAN
//#define RawBuffer Buffer<uint>
//#else
//#define RawBuffer ByteAddressBuffer
//#endif

#ifndef __cplusplus
//RawBuffer   BufferTable[]    : register(t0, space0);
//Texture1D   Texture1DTable[] : register(t0, space1);
Texture2D   Texture2DTable[] : register(t0, space0);
//Texture3D   Texture3DTable[] : register(t0, space3);
#endif

#endif // _DRIVER__HLSLI_