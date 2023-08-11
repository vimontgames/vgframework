#pragma once

namespace vg::gfx
{
    const core::uint bindless_texture_SRV_offset     = 0;
    const core::uint bindless_texture_SRV_count      = 16384;
    const core::uint bindless_texture_SRV_invalid    = bindless_texture_SRV_offset + bindless_texture_SRV_count - 1;
    const core::uint bindless_texture_SRV_binding    = 0;
                                                     
    const core::uint bindless_buffer_SRV_offset      = 16384;
    const core::uint bindless_buffer_SRV_count       = 16384;
    const core::uint bindless_buffer_SRV_invalid     = bindless_buffer_SRV_offset + bindless_buffer_SRV_count - 1;
    const core::uint bindless_buffer_SRV_binding     = 1;
                                                     
    const core::uint bindless_texture_UAV_offset     = 32768;
    const core::uint bindless_texture_UAV_count      = 16384;
    const core::uint bindless_texture_UAV_invalid    = bindless_texture_UAV_offset + bindless_texture_UAV_count - 1;
    const core::uint bindless_texture_UAV_binding    = 2;
                                                     
    const core::uint bindless_buffer_UAV_offset      = 49152;
    const core::uint bindless_buffer_UAV_count       = 16349;
    const core::uint bindless_buffer_UAV_invalid     = bindless_buffer_UAV_offset + bindless_buffer_UAV_count - 1;
    const core::uint bindless_buffer_UAV_binding     = 3;
                                                     
    const core::uint bindless_constantbuffer_offset  = 65500;
    const core::uint bindless_constantbuffer_count   = 35;
    const core::uint bindless_constantbuffer_invalid = bindless_constantbuffer_offset + bindless_constantbuffer_count - 1;
    const core::uint bindless_constantbuffer_binding = 4;
           
    const core::uint bindless_element_count          = bindless_texture_SRV_count + bindless_texture_UAV_offset + bindless_buffer_SRV_count + bindless_buffer_UAV_count + bindless_constantbuffer_count;
}