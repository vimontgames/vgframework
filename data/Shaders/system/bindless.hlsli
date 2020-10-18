#ifndef _BINDLESS__HLSLI_
#define _BINDLESS__HLSLI_

#ifdef __cplusplus
namespace vg::graphics::driver
{
    using namespace vg::core;

    const uint bindless_texture_SRV_offset  = 0;
    const uint bindless_texture_SRV_count   = 16384;
    const uint bindless_texture_SRV_invalid = bindless_texture_SRV_offset + bindless_texture_SRV_count - 1;
    const uint bindless_texture_SRV_binding = 0;

    const uint bindless_buffer_SRV_offset   = 16384;
    const uint bindless_buffer_SRV_count    = 16384;
    const uint bindless_buffer_SRV_invalid  = bindless_buffer_SRV_offset + bindless_buffer_SRV_count - 1;
    const uint bindless_buffer_SRV_binding  = 1;

    const uint bindless_texture_UAV_offset  = 32768;
    const uint bindless_texture_UAV_count   = 16384;
    const uint bindless_texture_UAV_invalid = bindless_texture_UAV_offset + bindless_texture_UAV_count - 1;
    const uint bindless_texture_UAV_binding = 0;

    const uint bindless_buffer_UAV_offset   = 49152;
    const uint bindless_buffer_UAV_count    = 16384;
    const uint bindless_buffer_UAV_invalid  = bindless_buffer_UAV_offset + bindless_buffer_UAV_count - 1;
    const uint bindless_buffer_UAV_binding  = 1;

    const uint bindless_element_count   = bindless_texture_SRV_count + bindless_texture_UAV_offset + bindless_buffer_SRV_count + bindless_buffer_UAV_offset;
}
#endif

#ifndef __cplusplus
#ifdef VULKAN
[[vk::binding(0, 0)]] Texture1D  Texture1DTable[];
[[vk::binding(0, 0)]] Texture2D  Texture2DTable[];
[[vk::binding(0, 0)]] Texture3D  Texture3DTable[];
[[vk::binding(1, 0)]] ByteAddressBuffer  BufferTable[];
#elif defined(DX12)
Texture1D         Texture1DTable[] : register(t0, space0);
Texture2D         Texture2DTable[] : register(t0, space0);
Texture3D         Texture3DTable[] : register(t0, space0);
ByteAddressBuffer BufferTable[]    : register(t16384, space1);
#endif // DX12
#endif // !__cplusplus
#endif // _DRIVER__HLSLI_