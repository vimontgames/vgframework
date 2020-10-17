#pragma once

#include "Shaders/system/bindless.hlsli"

namespace vg::graphics::driver
{
    struct BindlessHandle
    {
        using Type = core::u16;
        BindlessHandle(Type _value) : value(_value) { }
        operator Type() const { return value; }

    private:
        Type value;
    };

    struct BindlessTextureSrvHandle : public BindlessHandle
    {
        BindlessTextureSrvHandle(Type _value = bindless_texture_SRV_invalid) : BindlessHandle(_value) { }
    };

    struct BindlessTextureUAVHandle : public BindlessHandle
    {
        BindlessTextureUAVHandle(Type _value = bindless_texture_UAV_invalid) : BindlessHandle(_value) {  }
    };

    struct BindlessBufferSrvHandle : public BindlessHandle
    {
        BindlessBufferSrvHandle(Type _value = bindless_buffer_SRV_invalid) : BindlessHandle(_value) { }
    };

    struct BindlessBufferUAVHandle : public BindlessHandle
    {
        BindlessBufferUAVHandle(Type _value = bindless_buffer_UAV_invalid) : BindlessHandle(_value) { }
    };

    enum ReservedSlot : core::u32
    {
        None = 0x80000000
    };
}