#pragma once

#include "Shaders/system/bindless.hlsli.h"

namespace vg::gfx
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

    //struct BindlessConstantBufferHandle : public BindlessHandle
    //{
    //    BindlessConstantBufferHandle(Type _value = bindless_constantbuffer_invalid) : BindlessHandle(_value) { }
    //};

    enum ReservedSlot : core::u32
    {
        None = 0x80000000
    };
}