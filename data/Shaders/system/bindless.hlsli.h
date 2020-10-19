#pragma once

namespace vg::graphics::driver
{
    using namespace vg::core;

    const uint bindless_texture_SRV_offset = 0;
    const uint bindless_texture_SRV_count = 16384;
    const uint bindless_texture_SRV_invalid = bindless_texture_SRV_offset + bindless_texture_SRV_count - 1;
    const uint bindless_texture_SRV_binding = 0;

    const uint bindless_buffer_SRV_offset = 16384;
    const uint bindless_buffer_SRV_count = 16384;
    const uint bindless_buffer_SRV_invalid = bindless_buffer_SRV_offset + bindless_buffer_SRV_count - 1;
    const uint bindless_buffer_SRV_binding = 1;

    const uint bindless_texture_UAV_offset = 32768;
    const uint bindless_texture_UAV_count = 16384;
    const uint bindless_texture_UAV_invalid = bindless_texture_UAV_offset + bindless_texture_UAV_count - 1;
    const uint bindless_texture_UAV_binding = 0;

    const uint bindless_buffer_UAV_offset = 49152;
    const uint bindless_buffer_UAV_count = 16384;
    const uint bindless_buffer_UAV_invalid = bindless_buffer_UAV_offset + bindless_buffer_UAV_count - 1;
    const uint bindless_buffer_UAV_binding = 1;

    const uint bindless_element_count = bindless_texture_SRV_count + bindless_texture_UAV_offset + bindless_buffer_SRV_count + bindless_buffer_UAV_offset;
}